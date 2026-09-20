#include "desensamblar.h"


 char* mnemonico(uint8_t code) {
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

 char* nombreRegistro(uint8_t reg) {
    switch(reg) {
        case 0:  return "IP";
        case 1:  return "OPC";
        case 2:  return "OP1";
        case 3:  return "OP2";
        case 4:  return "LAR";
        case 5:  return "MAR";
        case 6:  return "MBR";
        case 10: return "EAX";
        case 11: return "EBX";
        case 12: return "ECX";
        case 13: return "EDX";
        case 14: return "EEX";
        case 15: return "EFX";
        case 16: return "AC";
        case 17: return "CC";
        case 26: return "CS";
        case 27: return "DS";
        default: return "XXX";
    }
}


void armarOperando(char *operando, uint8_t tipoOp, int32_t valor){
    uint8_t codRegistro;
    int desplazamiento;

    if(tipoOp == OPREG)
        sprintf(operando, "%s", nombreRegistro(valor & 0x1F)); /// 5 bits menos significativos representan el registro
    else
        if(tipoOp == OPIMM){
            sprintf(operando, "%04X", (valor & 0xFFFF));
            ///sprintf(operando, "%d", (valor & 0xFFFF)); /// 16 bits menos significativos (Valor decimal)
        }else
            if(tipoOp == OPMEM){    /// 16bits desplazamiento + 3 bits reservados + 5 cod registro
                desplazamiento = (valor >> 8) & 0xFFFF; /// 16bits
                codRegistro = valor & 0x1F;     /// 5bits
                if(desplazamiento == 0)
                    sprintf(operando, "[%s]", nombreRegistro(codRegistro));
                else
                    if (desplazamiento > 0)
                        sprintf(operando, "[%s+%d]", nombreRegistro(codRegistro), desplazamiento);
                    else
                        sprintf(operando, "[%s%d]", nombreRegistro(codRegistro), desplazamiento);
            }
}


void mostrarInstruccion(ETMaquinaVirtual *maqVirt, TRInstruction inst, int32_t dirFisicaInc, int32_t dirFisicaFin){
    int32_t longitud = dirFisicaFin - dirFisicaInc;
    char byteHexa[32] = {0}; /// Se inicializa la instruccion completa a mostrar
    char opA[32] = {0}, opB[32] = {0};
    int i, pos = 0;

    for(i = 0; i < longitud; i++){
        pos += sprintf(&byteHexa[pos], "%02X ", maqVirt->memoria[dirFisicaInc + i]);
    }

    if(inst.cantOperand == 2){
        armarOperando(opB, inst.tipoOpB, inst.opBValor);
        armarOperando(opA, inst.tipoOpA, inst.opAValor);
    }else
        if(inst.cantOperand == 1)
            armarOperando(opA, inst.tipoOpA, inst.opAValor);


    /// Imprimir formato: [0000] XX XX XX XX | MNEM OPA, OPB
    printf("[%04X] %-14s  | %-4s ", dirFisicaInc, byteHexa, mnemonico(inst.operacion));

    if (inst.cantOperand == 2)
        printf("%s, %s;\n", opA, opB);
    else
        if (inst.cantOperand == 1)
            printf("%s;\n", opA);
        else
            printf(";\n");

}


