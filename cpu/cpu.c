#include <stdlib.h>
#include "cpu.h"
#include "disassembler.h"
#include "opcode.h"
#include "sys.h"
#include "errores.h"

uint32_t armarRegistroOperando(int tipo, uint8_t *bytes) {
    uint32_t valor = 0;
    int tam = tamanioOperando(tipo);

    for (int i = 0; i < tam; i++) {
        valor = (valor << 8) | bytes[i];
    }

    return ((uint32_t)tipo << 24) | valor;
}

Instruccion buscarInstruccion(tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros) {
    Instruccion instr;

    /// Fetch de la instruccion: traduccion sin validacion de límite de datos ni modificación de LAR/MAR
    instr.dirInicio = traducir(registros[REGIP], tablaSegmentos, registros, 1, 0);
    uint8_t primerByte = memoria[instr.dirInicio];

    decodificarPrimerByte(primerByte, &instr.opcode, &instr.categoria);

    int tipoA, tipoB;
    extraerTiposOperando(primerByte, instr.categoria, &tipoA, &tipoB);

    int tamB = tamanioOperando(tipoB);
    int tamA = tamanioOperando(tipoA);

    /// En la codificacion de la maquina, los bytes del operando B anteceden a los de A
    uint8_t *bytesB = &memoria[instr.dirInicio + 1];
    uint8_t *bytesA = &memoria[instr.dirInicio + 1 + tamB];

    instr.operandoB = leerOperando(tipoB, bytesB);
    instr.operandoA = leerOperando(tipoA, bytesA);

    /// Actualizacion de registros de control de instruccion
    registros[REGOPC] = (uint32_t)instr.opcode;
    registros[REGOP1] = armarRegistroOperando(tipoA, bytesA);
    registros[REGOP2] = armarRegistroOperando(tipoB, bytesB);

    instr.largoTotal = 1 + tamB + tamA;
    uint16_t nuevoOffset = (uint16_t)(registros[REGIP] & 0xFFFF) + instr.largoTotal;
    registros[REGIP] = (registros[REGIP] & 0xFFFF0000) | nuevoOffset;

    return instr;
}

dirLogica calcularDirLogica(uint32_t regValor, int16_t desplazamiento) {
    if (regValor == 0xFFFFFFFF) {
        /// Registro de segmento inexistente o direccion nula (-1)
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }
    uint32_t segmento = regValor >> 16;
    int32_t offset = (int32_t)(uint16_t)(regValor & 0xFFFF) + desplazamiento;
    if (offset < 0 || offset > 0xFFFF) {
        /// Acceso por debajo de la base del segmento o desborde de 16 bits
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }
    return (segmento << 16) | (uint16_t)offset;
}

uint32_t leerValorOperando(Operando operando, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros) {
    dirLogica dirLog;
    dirFisica dirFis;

    switch (operando.tipo) {
        case TIPO_REGISTRO: {
            uint32_t valReg = registros[operando.codigoRegistro];
            switch (operando.sector) {
                case SECTOR_COMPLETO:
                    return valReg;

                case SECTOR_BYTE_BAJO: {
                    int8_t byteBajo = (int8_t)(valReg & 0xFF);
                    return (uint32_t)(int32_t)byteBajo;
                }

                case SECTOR_BYTE_ALTO: {
                    int8_t byteAlto = (int8_t)((valReg >> 8) & 0xFF);
                    return (uint32_t)(int32_t)byteAlto;
                }

                case SECTOR_WORD_BAJO: {
                    int16_t wordBajo = (int16_t)(valReg & 0xFFFF);
                    return (uint32_t)(int32_t)wordBajo;
                }

                default:
                    return valReg;
            }
        }

        case TIPO_INMEDIATO:
            return (uint32_t)(int32_t)operando.inmediato;

        case TIPO_MEMORIA: {
            dirLog = calcularDirLogica(registros[operando.codigoRegistro], operando.desplazamiento);
            dirFis = traducir(dirLog, tablaSegmentos, registros, operando.tamanoMemoria, 1);     // Carga LAR y MAR con la cantidad de bytes
            uint32_t valMem = leerDeMemoria(memoria, dirFis, operando.tamanoMemoria, registros); // Carga MBR

            switch (operando.tamanoMemoria) {
                case 1: {
                    int8_t byteVal = (int8_t)(valMem & 0xFF);
                    return (uint32_t)(int32_t)byteVal;
                }
                case 2: {
                    int16_t wordVal = (int16_t)(valMem & 0xFFFF);
                    return (uint32_t)(int32_t)wordVal;
                }
                case 4:
                default:
                    return valMem;
            }
        }

        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            return 0;
    }
}

void escribirValorOperando(Operando operando, uint32_t valor, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros) {
    dirLogica dirLog;
    dirFisica dirFis;

    switch (operando.tipo) {
        case TIPO_REGISTRO: {
            switch (operando.sector) {
                case SECTOR_COMPLETO:
                    registros[operando.codigoRegistro] = valor;
                    break;

                case SECTOR_BYTE_BAJO:
                    registros[operando.codigoRegistro] = (registros[operando.codigoRegistro] & 0xFFFFFF00u) | (valor & 0xFFu);
                    break;

                case SECTOR_BYTE_ALTO:
                    registros[operando.codigoRegistro] = (registros[operando.codigoRegistro] & 0xFFFF00FFu) | ((valor & 0xFFu) << 8);
                    break;

                case SECTOR_WORD_BAJO:
                    registros[operando.codigoRegistro] = (registros[operando.codigoRegistro] & 0xFFFF0000u) | (valor & 0xFFFFu);
                    break;

                default:
                    registros[operando.codigoRegistro] = valor;
                    break;
            }
            break;
        }

        case TIPO_MEMORIA: {
            dirLog = calcularDirLogica(registros[operando.codigoRegistro], operando.desplazamiento);
            dirFis = traducir(dirLog, tablaSegmentos, registros, operando.tamanoMemoria, 1);
            escribirEnMemoria(memoria, dirFis, valor, operando.tamanoMemoria, registros);
            break;
        }

        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            break;
    }
}

int hayMasInstrucciones(tabla_segmentos tablaSegmentos, Registros registros) {
    if (registros[REGCS] == 0xFFFFFFFF) {
        return 0;
    }
    int segCode = (int)(registros[REGCS] >> 16);
    int segIP = (int)(registros[REGIP] >> 16);
    uint16_t offsetIP = (uint16_t)(registros[REGIP] & 0xFFFF);

    if (!esSegmentoValido(tablaSegmentos, segCode)) {
        return 0;
    }

    return (segIP == segCode) && (offsetIP < obtenerTamanioSegmento(tablaSegmentos, segCode));
}

void saltarA(uint32_t desplazamiento, Registros registros) {
    registros[REGIP] = (registros[REGIP] & 0xFFFF0000) | (desplazamiento & 0xFFFF);
}

void ejecutarInstruccion(Instruccion instr, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros) {
    uint32_t valor;

    switch (instr.opcode) {
        /// Instrucciones con 2 operandos
        case OPCODE_MOV: {
            valor = leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            int tamA = obtenerTamanoOperando(instr.operandoA);
            uint32_t valEfectivo = truncarYExtenderSigno(valor, tamA);
            actualizarFlagsValor(valEfectivo, registros);
            break;
        }

        case OPCODE_ADD:
            valor = sumar(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                          leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_SUB:
            valor = restar(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                           leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_MUL:
            valor = multiplicar(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                                leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_DIV:
            valor = dividir(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                            leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_CMP:
            restar(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                   leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            break;

        case OPCODE_AND:
            valor = and_logico(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                               leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_OR:
            valor = or_logico(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                              leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_XOR:
            valor = xor_logico(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                               leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_SWAP: {
            uint32_t valA = leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros);
            uint32_t valB = leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros);

            escribirValorOperando(instr.operandoA, valB, tablaSegmentos, memoria, registros);
            escribirValorOperando(instr.operandoB, valA, tablaSegmentos, memoria, registros);

            xor_logico(valA, valB, registros);
            break;
        }

        case OPCODE_SHL:
            valor = shift_izq(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                              leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_SHR:
            valor = shift_der(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                              leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_SAR:
            valor = shift_der_aritmetico(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                                         leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_LDL:
            valor = LDL(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                        (uint16_t)leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros));
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_LDH:
            valor = LDH(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros),
                        (uint16_t)leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros));
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        case OPCODE_RND: {
            uint32_t limite = leerValorOperando(instr.operandoB, tablaSegmentos, memoria, registros);
            valor = (limite == 0xFFFFFFFF) ? (uint32_t)rand() : (uint32_t)(rand() % (limite + 1));
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;
        }

        /// Instrucciones con 1 operando
        case OPCODE_SYS:
            llamadaSistema(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), memoria, tablaSegmentos, registros);
            break;

        case OPCODE_JMP:
            saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            break;

        case OPCODE_JP:
            if ((registros[REGCC] & NMASK) == 0 && (registros[REGCC] & ZMASK) == 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_JN:
            if ((registros[REGCC] & NMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_JZ:
            if ((registros[REGCC] & ZMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_JC:
            if ((registros[REGCC] & CMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_JV:
            if ((registros[REGCC] & VMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_JNP:
            if ((registros[REGCC] & NMASK) != 0 || (registros[REGCC] & ZMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_JNN:
            if ((registros[REGCC] & NMASK) == 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_JNZ:
            if ((registros[REGCC] & ZMASK) == 0) {
                saltarA(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            }
            break;

        case OPCODE_NOT:
            valor = not_logico(leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros), registros);
            escribirValorOperando(instr.operandoA, valor, tablaSegmentos, memoria, registros);
            break;

        /// Pila y subrutinas
        case OPCODE_PUSH: {
            uint32_t valPush = leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros);
            push(valPush, tablaSegmentos, memoria, registros);
            break;
        }

        case OPCODE_POP: {
            uint32_t valPop = pop(tablaSegmentos, memoria, registros);
            escribirValorOperando(instr.operandoA, valPop, tablaSegmentos, memoria, registros);
            break;
        }

        case OPCODE_CALL: {
            uint32_t destino = leerValorOperando(instr.operandoA, tablaSegmentos, memoria, registros);
            push(registros[REGIP], tablaSegmentos, memoria, registros);
            saltarA(destino, registros);
            break;
        }

        /// Instrucciones sin operandos
        case OPCODE_RET:
            registros[REGIP] = pop(tablaSegmentos, memoria, registros);
            break;

        case OPCODE_STOP:
            /// STOP asigna -1 (0xFFFFFFFF) a IP para indicar terminacion
            registros[REGIP] = 0xFFFFFFFF;
            break;

        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            break;
    }
}

void ejecutarPrograma(tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros, int flagD) {
    while (hayMasInstrucciones(tablaSegmentos, registros)) {
        Instruccion instr = buscarInstruccion(tablaSegmentos, memoria, registros);
        if(flagD){
            mostrarInstruccion(instr, memoria);
        }
        ejecutarInstruccion(instr, tablaSegmentos, memoria, registros);
    }
}

uint32_t LDH(uint32_t valorBase, uint16_t parteAlta) {
    valorBase &= 0x0000FFFF;               // Conserva la parte baja
    valorBase |= ((uint32_t)parteAlta << 16); // Carga la nueva parte alta
    return valorBase;
}

uint32_t LDL(uint32_t valorBase, uint16_t parteBaja) {
    valorBase &= 0xFFFF0000;               // Conserva la parte alta
    valorBase |= (uint32_t)parteBaja;      // Carga la nueva parte baja
    return valorBase;
}
