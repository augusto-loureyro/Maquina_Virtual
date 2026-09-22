#include "desensamblar.h"


char* mnemonico(int code) {
    switch(code) {
        case 0x00: return "SYS";
        case 0x01: return "JMP";
        case 0x02: return "JP";
        case 0x03: return "JN";
        case 0x04: return "JZ";
        case 0x05: return "JC";
        case 0x06: return "JV";
        case 0x07: return "JNP";
        case 0x08: return "JNN";
        case 0x09: return "JNZ";
        case 0x0A: return "NOT";
        case 0x0F: return "STOP";
        case 0x10: return "MOV";
        case 0x11: return "ADD";
        case 0x12: return "SUB";
        case 0x13: return "MUL";
        case 0x14: return "DIV";
        case 0x15: return "CMP";
        case 0x16: return "AND";
        case 0x17: return "OR";
        case 0x18: return "XOR";
        case 0x19: return "SWAP";
        case 0x1A: return "SHL";
        case 0x1B: return "SHR";
        case 0x1C: return "SAR";
        case 0x1D: return "LDL";
        case 0x1E: return "LDH";
        case 0x1F: return "RND";
        default:   return "XXX";
    }
}

char* nombreRegistro(int reg) {
    switch(reg) {
        case REGIP:  return "IP";
        case REGOPC:  return "OPC";
        case REGOP1:  return "OP1";
        case REGOP2:  return "OP2";
        case REGLAR:  return "LAR";
        case REGMAR:  return "MAR";
        case REGMBR:  return "MBR";
        case REGEAX: return "EAX";
        case REGEBX: return "EBX";
        case REGECX: return "ECX";
        case REGEDX: return "EDX";
        case REGEEX: return "EEX";
        case REGEFX: return "EFX";
        case REGAC: return "AC";
        case REGCC: return "CC";
        case REGCS: return "CS";
        case REGDS: return "DS";
        default: return "XXX";
    }
}


void armarOperando(char *operando, ETMaquinaVirtual *maqVirt, int op) {
    int desplazamiento, tipoOp;

    tipoOp = (op >> 24) & 0xFF;

    if (tipoOp == OPREG)
        sprintf(operando, "%s", nombreRegistro(operandoDest(maqVirt, tipoOp, op)));
    else
        if (tipoOp == OPIMM)
            sprintf(operando, "%02X", operandoDest(maqVirt, tipoOp, op));
        else {
            desplazamiento = (op >> 8) & 0xFFFF;

            if (desplazamiento == 0)
                sprintf(operando, "[%s]", nombreRegistro(operandoDest(maqVirt, OPREG, op)));
            else
                if (desplazamiento > 0)
                    sprintf(operando, "[%s+%d]", nombreRegistro(operandoDest(maqVirt, OPREG, op)), desplazamiento);
                else
                    sprintf(operando, "[%s-%d]", nombreRegistro(operandoDest(maqVirt, OPREG, op)), desplazamiento);
        }
}


void mostrarInstruccion(ETMaquinaVirtual *maqVirt, unsigned int dirFisicaInc, unsigned int dirFisicaFin) {
    char byteHexa[30] = {0}, opA[30] = {0}, opB[30] = {0};
    int i, pos = 0;

    for(i = 0; dirFisicaInc + i < dirFisicaFin; i++)
        pos += sprintf(&byteHexa[pos], "%02X ", leerByteDirFisica(maqVirt, dirFisicaInc + i)); // Retorna cant caracteres escritos ("XX " => 3)

    if (maqVirt->memoria[REGOP1] != 0) {
        if (maqVirt->memoria[REGOP2] != 0)
            armarOperando(opB, maqVirt, maqVirt->memoria[REGOP2]);
        armarOperando(opA, maqVirt, maqVirt->memoria[REGOP1]);
    }

    /// Imprimir formato: [0000] XX XX XX XX | MNEM OPA, OPB
    printf("[%04X] %-14s  | %-4s ", dirFisicaInc, byteHexa, mnemonico(maqVirt->registros[REGOPC]));

    if (maqVirt->memoria[REGOP1] != 0) {
        if (maqVirt->memoria[REGOP2] != 0)
            printf("%s, %s", opA, opB);
        else
            printf("%s", opA);
    }

    printf("\n");
}
