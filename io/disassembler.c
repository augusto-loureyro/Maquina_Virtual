#include <stdio.h>
#include <string.h>
#include "disassembler.h"
#include "cpu.h"
#include "memoria.h"
#include "registros.h"
#include "opcode.h"
#include "traductor.h"

static void formatearOperando(Operando operando, char *buf, size_t tam) {
    switch (operando.tipo) {
        case TIPO_REGISTRO:
            if (operando.codigoRegistro >= REGEAX && operando.codigoRegistro <= REGEFX) {
                char regChar = (char)('A' + (operando.codigoRegistro - REGEAX));
                switch (operando.sector) {
                    case SECTOR_COMPLETO:
                        snprintf(buf, tam, "E%cX", regChar);
                        break;
                    case SECTOR_BYTE_BAJO:
                        snprintf(buf, tam, "%cL", regChar);
                        break;
                    case SECTOR_BYTE_ALTO:
                        snprintf(buf, tam, "%cH", regChar);
                        break;
                    case SECTOR_WORD_BAJO:
                        snprintf(buf, tam, "%cX", regChar);
                        break;
                    default:
                        snprintf(buf, tam, "E%cX", regChar);
                        break;
                }
            } else {
                snprintf(buf, tam, "%s", nombreRegistro(operando.codigoRegistro));
            }
            break;

        case TIPO_INMEDIATO:
            snprintf(buf, tam, "%d", operando.inmediato);
            break;

        case TIPO_MEMORIA: {
            const char *pref = "";
            if (operando.tamanoMemoria == 1) {
                pref = "b";
            } else if (operando.tamanoMemoria == 2) {
                pref = "w";
            } else if (operando.tamanoMemoria == 4 && operando.modifMemoria == MODIF_MEM_LONG) {
                pref = "l";
            }

            if (operando.desplazamiento >= 0)
                snprintf(buf, tam, "%s[%s+%d]", pref, nombreRegistro(operando.codigoRegistro), operando.desplazamiento);
            else
                snprintf(buf, tam, "%s[%s%d]", pref, nombreRegistro(operando.codigoRegistro), operando.desplazamiento);
            break;
        }

        default:
            buf[0] = '\0';
            break;
    }
}

void mostrarInstruccion(Instruccion instr, Memoria memoria, int esEntryPoint) {
    ///
    putchar(esEntryPoint ? '>' : ' ');
    /// 1. Direccion fisica de inicio: [0000]
    printf("[%04X] ", (unsigned)instr.dirInicio);

    /// 2. Bytes en hexadecimal
    int caracteresHex = 0;
    for (int i = 0; i < instr.largoTotal; i++) {
        printf("%02X ", memoria[instr.dirInicio + i]);
        caracteresHex += 3;
    }

    /// Alineacion hasta la barra '|' (espacio para hasta 7-8 bytes)
    const int anchoHex = 23;
    for (int i = caracteresHex; i < anchoHex; i++) {
        putchar(' ');
    }
    printf("| ");

    /// 3. Mnemonico
    printf("%-5s", mnemonico(instr.opcode));

    /// 4. Operandos formateados
    char opA[64] = "";
    char opB[64] = "";

    switch (instr.categoria) {
        case DOS_OPERANDOS:
            formatearOperando(instr.operandoA, opA, sizeof(opA));
            formatearOperando(instr.operandoB, opB, sizeof(opB));
            printf(" %s, %s", opA, opB);
            break;

        case UN_OPERANDO:
            formatearOperando(instr.operandoA, opA, sizeof(opA));
            printf(" %s", opA);
            break;

        case SIN_OPERANDOS:
            /// STOP no lleva operandos
            break;
    }

    printf("\n");
}

#define ANCHO_HEX 23     /// mismo ancho que usa mostrarInstruccion antes de la barra

/** Muestra las constantes string del Const Segment: [dir] bytes | "texto" */
void mostrarConstantes(Memoria memoria, tabla_segmentos tabla, Registros registros) {
    /// KS == CS cubre el caso V1, donde KS puede quedar en 0 (segmento de codigo)
    if (registros[REGKS] == 0xFFFFFFFF || registros[REGKS] == registros[REGCS])
        return;

    int seg = (int)(registros[REGKS] >> 16);
    uint32_t dir = obtenerBaseSegmento(tabla, seg);
    uint32_t fin = dir + obtenerTamanioSegmento(tabla, seg);

    while (dir < fin) {
        uint32_t i = dir;
        while (i < fin && memoria[i] != 0)
            i++;
        uint32_t total = (i - dir) + (i < fin ? 1 : 0);      /// bytes de la cadena, con el '\0'

        printf("[%04X] ", (unsigned)dir);
        uint32_t mostrar = (total > 7) ? 6 : total;
        int cols = 0;
        for (uint32_t k = 0; k < mostrar; k++) {
            printf("%02X ", memoria[dir + k]);
            cols += 3;
        }
        if (total > 7) {
            printf(".. ");
            cols += 3;
        }
        while (cols++ < ANCHO_HEX)
            putchar(' ');

        printf("| \"");
        for (uint32_t k = dir; k < i; k++)
            putchar((memoria[k] >= 32 && memoria[k] <= 126) ? memoria[k] : '.');
        printf("\"\n");

        dir += total;
    }
}

/** Desensamblado completo: constantes y luego todo el Code Segment, con '>' en el IP inicial */
void mostrarDisassembly(Memoria memoria, tabla_segmentos tabla, Registros registros) {
    Registros copia;
    memcpy(copia, registros, CANT_REGISTROS * sizeof(uint32_t));   /// recorrer sin tocar el IP real

    mostrarConstantes(memoria, tabla, registros);

    if (registros[REGCS] == 0xFFFFFFFF)
        return;

    uint32_t dirEntrada = traducir(registros[REGIP], tabla, registros, 0, 0);
    copia[REGIP] = registros[REGCS];                               /// empezar desde el comienzo del Code Segment

    while (hayMasInstrucciones(tabla, copia)) {
        Instruccion instr = buscarInstruccion(tabla, memoria, copia);
        mostrarInstruccion(instr, memoria, instr.dirInicio == dirEntrada);
    }
}

const char* mnemonico(int opcode) {
    switch (opcode) {
        case OPCODE_SYS:  return "SYS";
        case OPCODE_JMP:  return "JMP";
        case OPCODE_JP:   return "JP";
        case OPCODE_JN:   return "JN";
        case OPCODE_JZ:   return "JZ";
        case OPCODE_JC:   return "JC";
        case OPCODE_JV:   return "JV";
        case OPCODE_JNP:  return "JNP";
        case OPCODE_JNN:  return "JNN";
        case OPCODE_JNZ:  return "JNZ";
        case OPCODE_NOT:  return "NOT";
        case OPCODE_PUSH: return "PUSH";
        case OPCODE_POP:  return "POP";
        case OPCODE_CALL: return "CALL";
        case OPCODE_RET:  return "RET";
        case OPCODE_STOP: return "STOP";
        case OPCODE_MOV:  return "MOV";
        case OPCODE_ADD:  return "ADD";
        case OPCODE_SUB:  return "SUB";
        case OPCODE_MUL:  return "MUL";
        case OPCODE_DIV:  return "DIV";
        case OPCODE_CMP:  return "CMP";
        case OPCODE_AND:  return "AND";
        case OPCODE_OR:   return "OR";
        case OPCODE_XOR:  return "XOR";
        case OPCODE_SWAP: return "SWAP";
        case OPCODE_SHL:  return "SHL";
        case OPCODE_SHR:  return "SHR";
        case OPCODE_SAR:  return "SAR";
        case OPCODE_LDL:  return "LDL";
        case OPCODE_LDH:  return "LDH";
        case OPCODE_RND:  return "RND";
        default:          return "???";
    }
}

const char* nombreRegistro(int reg) {
    switch (reg) {
        case REGIP:  return "IP";
        case REGOPC: return "OPC";
        case REGOP1: return "OP1";
        case REGOP2: return "OP2";
        case REGLAR: return "LAR";
        case REGMAR: return "MAR";
        case REGMBR: return "MBR";
        case REGSP:  return "SP";
        case REGBP:  return "BP";
        case REGEAX: return "EAX";
        case REGEBX: return "EBX";
        case REGECX: return "ECX";
        case REGEDX: return "EDX";
        case REGEEX: return "EEX";
        case REGEFX: return "EFX";
        case REGAC:  return "AC";
        case REGCC:  return "CC";
        case REGCS:  return "CS";
        case REGDS:  return "DS";
        case REGES:  return "ES";
        case REGSS:  return "SS";
        case REGKS:  return "KS";
        case REGPS:  return "PS";
        default:     return "???";
    }
}
