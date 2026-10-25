#include <stdio.h>
#include <string.h>
#include "disassembler.h"
#include "cpu.h"
#include "memoria.h"
#include "registros.h"
#include "opcode.h"

static void formatearOperando(Operando o, char *buf, size_t tam) {
    switch (o.tipo) {
        case TIPO_REGISTRO:
            snprintf(buf, tam, "%s", nombreRegistro(o.codigoRegistro));
            break;
        case TIPO_INMEDIATO:
            snprintf(buf, tam, "%d", o.inmediato);
            break;
        case TIPO_MEMORIA:
            if (o.desplazamiento >= 0)
                snprintf(buf, tam, "[%s+%d]", nombreRegistro(o.codigoRegistro), o.desplazamiento);
            else
                snprintf(buf, tam, "[%s%d]", nombreRegistro(o.codigoRegistro), o.desplazamiento);
            break;
        default:
            buf[0] = '\0';
            break;
    }
}

void mostrarInstruccion(Instruccion instr, Memoria m) {
    /// 1. Direccion fisica de inicio: [0000]
    printf("[%04X] ", (unsigned)instr.dirInicio);

    /// 2. Bytes en hexadecimal
    int caracteresHex = 0;
    for (int i = 0; i < instr.largoTotal; i++) {
        printf("%02X ", m[instr.dirInicio + i]);
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
        default:     return "???";
    }
}
