#include "desensamblar.h"


char* mnemonico(uint8_t code){
    char mnem[5];
    switch(code){
        case 0x00:
            strcpy(mnem,"SYS");
            break;
        case 0x01:
            strcpy(mnem,"JMP");
            break;
        case 0x02:
            strcpy(mnem,"JP");
            break;
        case 0x03:
            strcpy(mnem,"JN");
            break;
        case 0x04:
            strcpy(mnem,"JZ");
            break;
        case 0x05:
            strcpy(mnem,"JC");
            break;
        case 0x06:
            strcpy(mnem,"JV");
            break;
        case 0x07:
            strcpy(mnem,"JNP");
            break;
        case 0x08:
            strcpy(mnem,"JNN");
            break;
        case 0x09:
            strcpy(mnem,"JNZ");
            break;
        case 0x0A:
            strcpy(mnem,"NOT");
            break;
        case 0x0F:
            strcpy(mnem,"STOP");
            break;
        case 0x10:
            strcpy(mnem,"MOV");
            break;
        case 0x11:
            strcpy(mnem,"ADD");
            break;
        case 0x12:
            strcpy(mnem,"SUB");
            break;
        case 0x13:
            strcpy(mnem,"MUL");
            break;
        case 0x14:
            strcpy(mnem,"DIV");
            break;
        case 0x15:
            strcpy(mnem,"CMP");
            break;
        case 0x16:
            strcpy(mnem,"AND");
            break;
        case 0x17:
            strcpy(mnem,"OR");
            break;
        case 0x18:
            strcpy(mnem,"XOR");
            break;
        case 0x19:
            strcpy(mnem,"SWAP");
            break;
        case 0x1A:
            strcpy(mnem,"SHL");
            break;
        case 0x1B:
            strcpy(mnem,"SHR");
            break;
        case 0x1C:
            strcpy(mnem,"SAR");
            break;
        case 0x1D:
            strcpy(mnem,"LDL");
            break;
        case 0x1E:
            strcpy(mnem,"LDH");
            break;
        case 0x1F:
            strcpy(mnem,"RND");
            break;
        default:
            strcpy(mnem,"XXX");
    }
    return mnem;
}

char* nombreRegistro(uint8_t reg){
    char nombre[5];
    switch(reg){
        case 0:
            strcpy(nombre,"IP");
            break;
        case 1:
            strcpy(nombre, "OPC");
            break;
        case 2:
            strcpy(nombre, "OP1");
            break;
        case 3:
            strcpy(nombre, "OP2");
            break;
        case 4:
            strcpy(nombre, "LAR");
            break;
        case 5:
            strcpy(nombre, "MAR");
            break;
        case 6:
            strcpy(nombre, "MBR");
            break;
        case 10:
            strcpy(nombre, "EAX");
            break;
        case 11:
            strcpy(nombre, "EBX");
            break;
        case 12:
            strcpy(nombre, "ECX");
            break;
        case 13:
            strcpy(nombre, "EDX");
            break;
        case 14:
            strcpy(nombre, "EEX");
            break;
        case 15:
            strcpy(nombre, "EFX");
            break;
        case 16:
            strcpy(nombre, "AC");
            break;
        case 17:
            strcpy(nombre, "CC");
            break;
        case 26:
            strcpy(nombre, "CS");
            break;
        case 27:
            strcpy(nombre, "DS");
            break;
        default:
            strcpy(nombre, "XXX");
    }
    return nombre;
}


void armarOperando(char *operando, uint8_t tipoOp, int32_t valor){
    uint8_t codRegistro;
    int desplazamiento;

    if(tipoOp == OPREG)
        sprintf(operando, "%s", nombreRegistro(valor & 0x1F)); /// 5 bits menos significativos representan el registro
    else
        if(tipoOp == OPIMM)
            sprintf(operando, "%d", (valor & 0xFFFF)); /// 16 bits menos significativos (Valor decimal)
        else
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
    printf("[%04X] %-14s | %-4s ", dirFisicaInc, byteHexa, mnemonico(inst.operacion));

    if (inst.cantOperand == 2)
        printf("%s, %s;\n", opA, opB);
    else
        if (inst.cantOperand == 1)
            printf("%s;\n", opA);
        else
            printf(";\n");

}


