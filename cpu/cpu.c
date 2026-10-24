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

Instruccion buscarInstruccion(tabla_segmentos t, Memoria m, Registros r) {
    Instruccion instr;

    /// Fetch de la instruccion: traduccion sin validacion de límite de datos ni modificación de LAR/MAR
    instr.dirInicio = traducir(r[REGIP], t, r, 1, 0);
    uint8_t primerByte = m[instr.dirInicio];

    decodificarPrimerByte(primerByte, &instr.opcode, &instr.categoria);

    int tipoA, tipoB;
    extraerTiposOperando(primerByte, instr.categoria, &tipoA, &tipoB);

    int tamB = tamanioOperando(tipoB);
    int tamA = tamanioOperando(tipoA);

    /// En la codificacion de la maquina, los bytes del operando B anteceden a los de A
    uint8_t *bytesB = &m[instr.dirInicio + 1];
    uint8_t *bytesA = &m[instr.dirInicio + 1 + tamB];

    instr.operandoB = leerOperando(tipoB, bytesB);
    instr.operandoA = leerOperando(tipoA, bytesA);

    /// Actualizacion de registros de control de instruccion
    r[REGOPC] = (uint32_t)instr.opcode;
    r[REGOP1] = armarRegistroOperando(tipoA, bytesA);
    r[REGOP2] = armarRegistroOperando(tipoB, bytesB);

    instr.largoTotal = 1 + tamB + tamA;
    uint16_t nuevoOffset = (uint16_t)(r[REGIP] & 0xFFFF) + instr.largoTotal;
    r[REGIP] = (r[REGIP] & 0xFFFF0000) | nuevoOffset;

    return instr;
}

static dirLogica calcularDirLogica(uint32_t regValor, int16_t desplazamiento) {
    uint32_t segmento = regValor >> 16;
    int32_t offset = (int32_t)(uint16_t)(regValor & 0xFFFF) + desplazamiento;
    if (offset < 0) {
        /// Acceso por debajo de la base del segmento
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }
    return (segmento << 16) | (uint16_t)offset;
}

uint32_t leerValorOperando(Operando o, tabla_segmentos t, Memoria m, Registros r) {
    dirLogica l;
    dirFisica f;

    switch (o.tipo) {
        case TIPO_REGISTRO:
            return r[o.codigoRegistro];

        case TIPO_INMEDIATO:
            return (uint32_t)(int32_t)o.inmediato;

        case TIPO_MEMORIA:
            l = calcularDirLogica(r[o.codigoRegistro], o.desplazamiento);
            f = traducir(l, t, r, TAMANIO_DATO, 1);     // Carga LAR y MAR
            return leerDeMemoria(m, f, TAMANIO_DATO, r); // Carga MBR

        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            return 0;
    }
}

void escribirValorOperando(Operando o, uint32_t valor, tabla_segmentos t, Memoria m, Registros r) {
    dirLogica l;
    dirFisica f;

    switch (o.tipo) {
        case TIPO_REGISTRO:
            r[o.codigoRegistro] = valor;
            break;

        case TIPO_MEMORIA:
            l = calcularDirLogica(r[o.codigoRegistro], o.desplazamiento);
            f = traducir(l, t, r, TAMANIO_DATO, 1);
            escribirEnMemoria(m, f, valor, TAMANIO_DATO, r);
            break;

        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            break;
    }
}

int hayMasInstrucciones(tabla_segmentos t, Registros r) {
    int segmento = r[REGIP] >> 16;
    uint16_t offset = r[REGIP] & 0xFFFF;

    return (segmento == CODE) && (offset < obtenerTamanioSegmento(t, CODE));
}

void saltarA(uint32_t desplazamiento, Registros r) {
    r[REGIP] = (r[REGIP] & 0xFFFF0000) | (desplazamiento & 0xFFFF);
}

void ejecutarInstruccion(Instruccion instr, tabla_segmentos t, Memoria m, Registros r) {
    uint32_t valor;

    switch (instr.opcode) {
        /// Instrucciones con 2 operandos
        case OPCODE_MOV:
            valor = leerValorOperando(instr.operandoB, t, m, r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            actualizarFlagsValor(valor, r);
            break;

        case OPCODE_ADD:
            valor = sumar(leerValorOperando(instr.operandoA, t, m, r),
                          leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_SUB:
            valor = restar(leerValorOperando(instr.operandoA, t, m, r),
                           leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_MUL:
            valor = multiplicar(leerValorOperando(instr.operandoA, t, m, r),
                                leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_DIV:
            valor = dividir(leerValorOperando(instr.operandoA, t, m, r),
                            leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_CMP:
            restar(leerValorOperando(instr.operandoA, t, m, r),
                   leerValorOperando(instr.operandoB, t, m, r), r);
            break;

        case OPCODE_AND:
            valor = and_logico(leerValorOperando(instr.operandoA, t, m, r),
                               leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_OR:
            valor = or_logico(leerValorOperando(instr.operandoA, t, m, r),
                              leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_XOR:
            valor = xor_logico(leerValorOperando(instr.operandoA, t, m, r),
                               leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_SWAP: {
            uint32_t valA = leerValorOperando(instr.operandoA, t, m, r);
            uint32_t valB = leerValorOperando(instr.operandoB, t, m, r);

            /// Operaciones equivalentes: XOR A, B; XOR B, A; XOR A, B
            uint32_t paso1 = xor_logico(valA, valB, r);
            escribirValorOperando(instr.operandoA, paso1, t, m, r);

            uint32_t paso2 = xor_logico(paso1, valB, r);
            escribirValorOperando(instr.operandoB, paso2, t, m, r);

            uint32_t paso3 = xor_logico(paso1, paso2, r);
            escribirValorOperando(instr.operandoA, paso3, t, m, r);
            break;
        }

        case OPCODE_SHL:
            valor = shift_izq(leerValorOperando(instr.operandoA, t, m, r),
                              leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_SHR:
            valor = shift_der(leerValorOperando(instr.operandoA, t, m, r),
                              leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_SAR:
            valor = shift_der_aritmetico(leerValorOperando(instr.operandoA, t, m, r),
                                         leerValorOperando(instr.operandoB, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_LDL:
            valor = LDL(leerValorOperando(instr.operandoA, t, m, r),
                        (uint16_t)leerValorOperando(instr.operandoB, t, m, r));
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_LDH:
            valor = LDH(leerValorOperando(instr.operandoA, t, m, r),
                        (uint16_t)leerValorOperando(instr.operandoB, t, m, r));
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        case OPCODE_RND: {
            uint32_t limite = leerValorOperando(instr.operandoB, t, m, r);
            valor = (limite == 0xFFFFFFFF) ? (uint32_t)rand() : (uint32_t)(rand() % (limite + 1));
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;
        }

        /// Instrucciones con 1 operando
        case OPCODE_SYS:
            llamadaSistema(leerValorOperando(instr.operandoA, t, m, r), m, t, r);
            break;

        case OPCODE_JMP:
            saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            break;

        case OPCODE_JP:
            if ((r[REGCC] & NMASK) == 0 && (r[REGCC] & ZMASK) == 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_JN:
            if ((r[REGCC] & NMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_JZ:
            if ((r[REGCC] & ZMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_JC:
            if ((r[REGCC] & CMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_JV:
            if ((r[REGCC] & VMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_JNP:
            if ((r[REGCC] & NMASK) != 0 || (r[REGCC] & ZMASK) != 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_JNN:
            if ((r[REGCC] & NMASK) == 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_JNZ:
            if ((r[REGCC] & ZMASK) == 0) {
                saltarA(leerValorOperando(instr.operandoA, t, m, r), r);
            }
            break;

        case OPCODE_NOT:
            valor = not_logico(leerValorOperando(instr.operandoA, t, m, r), r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;

        /// Instrucciones sin operandos
        case OPCODE_STOP:
            /// STOP asigna -1 (0xFFFFFFFF) a IP para indicar terminacion
            r[REGIP] = 0xFFFFFFFF;
            break;

        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            break;
    }
}

void ejecutarPrograma(tabla_segmentos t, Memoria m, Registros r, int flagD) {
    while (hayMasInstrucciones(t, r)) {
        Instruccion instr = buscarInstruccion(t, m, r);
        if(flagD){
            mostrarInstruccion(instr, m);
        }
        ejecutarInstruccion(instr, t, m, r);
    }
}

uint32_t LDH(uint32_t b, uint16_t h) {
    b &= 0x0000FFFF;               // Conserva la parte baja
    b |= ((uint32_t)h << 16);      // Carga la nueva parte alta
    return b;
}

uint32_t LDL(uint32_t b, uint16_t l) {
    b &= 0xFFFF0000;               // Conserva la parte alta
    b |= (uint32_t)l;              // Carga la nueva parte baja
    return b;
}
