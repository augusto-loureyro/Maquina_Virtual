#include "desensamblar.h"


char* mnemonico(int code) {
    switch(code) {
        case SYS: return "SYS";
        case JMP: return "JMP";
        case JP: return "JP";
        case JN: return "JN";
        case JZ: return "JZ";
        case JC: return "JC";
        case JV: return "JV";
        case JNP: return "JNP";
        case JNN: return "JNN";
        case JNZ: return "JNZ";
        case NOT: return "NOT";
        case STOP: return "STOP";
        case MOV: return "MOV";
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case CMP: return "CMP";
        case AND: return "AND";
        case OR: return "OR";
        case XOR: return "XOR";
        case SWAP: return "SWAP";
        case SHL: return "SHL";
        case SHR: return "SHR";
        case SAR: return "SAR";
        case LDL: return "LDL";
        case LDH: return "LDH";
        case RND: return "RND";
        default: return "XXX";
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
    //printf("%X ", op);
    //printf("%X ", tipoOp);

    if (tipoOp == OPREG)
        sprintf(operando, "%s", nombreRegistro(operandoDest(maqVirt, tipoOp, op)));
    else
        if (tipoOp == OPIMM)
            sprintf(operando, "%d", operandoDest(maqVirt, tipoOp, op));
        else {
            desplazamiento = (op >> 8) & 0xFFFF;
            //printf("%d", desplazamiento);

            if (desplazamiento < 0)
                sprintf(operando, "[%s-%d]", nombreRegistro(operandoDest(maqVirt, OPREG, op)), desplazamiento);
            else
                if(strcmp(nombreRegistro(operandoDest(maqVirt, OPREG, op)), "DS") == 0)
                    sprintf(operando, "[%d]", desplazamiento);
                else
                    sprintf(operando, "[%s+%d]", nombreRegistro(operandoDest(maqVirt, OPREG, op)), desplazamiento);
        }
}


void mostrarInstruccion(ETMaquinaVirtual *maqVirt, unsigned int dirFisicaInc, unsigned int dirFisicaFin) {
    char byteHexa[30] = {0}, opA[30] = {0}, opB[30] = {0};
    int i, pos = 0;

    for(i = 0; dirFisicaInc + i < dirFisicaFin; i++)
        pos += sprintf(&byteHexa[pos], "%02X ", (uint8_t)leerByteDirFisica(maqVirt, dirFisicaInc + i)); /// Retorna cant caracteres escritos ("XX " => 3)

    if (maqVirt->registros[REGOP1] != 0) {
        if (maqVirt->registros[REGOP2] != 0)
            armarOperando(opB, maqVirt, maqVirt->registros[REGOP2]);
        armarOperando(opA, maqVirt, maqVirt->registros[REGOP1]);
    }

    /// Imprimir formato: [0000] XX XX XX XX | MNEM OPA, OPB
    printf("[%04X] %-25s  |  %-4s ", dirFisicaInc, byteHexa, mnemonico(maqVirt->registros[REGOPC]));

    if (maqVirt->registros[REGOP1] != 0) {
        if (maqVirt->registros[REGOP2] != 0)
            printf("%-s, %-s", opA, opB);
        else
            printf("%-s", opA);
    }

    printf("\n");
}
