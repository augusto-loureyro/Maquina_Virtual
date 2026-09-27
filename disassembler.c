// disassembler.c
// Muestra en pantalla la representación en lenguaje ensamblador de cada
// instrucción ejecutada, cuando se invoca la VM con el flag -d.
// Reutiliza la instrucción ya decodificada por buscarInstruccion (cpu.c);
// no vuelve a decodificar nada por su cuenta.

#include <stdio.h>
#include "disassembler.h"
#include "cpu.h"
#include "memoria.h"
#include "registros.h"
#include "opcode.h"

 

// Muestra un solo operando según su tipo.
// TIPO_NINGUNO no debería llegar nunca a esta función: mostrarInstruccion
// solo la llama para los operandos que realmente existen según la categoría
// de la instrucción.
void imprimirOperando(Operando o){
    char buffer[64];

    switch (o.tipo){
        case TIPO_REGISTRO:
            printf("%20s", nombreRegistro(o.codigoRegistro));
            break;

        case TIPO_INMEDIATO:
            printf("%20d", o.inmediato);
            break;

        case TIPO_MEMORIA:
            if (o.desplazamiento >= 0)
                snprintf(buffer, sizeof(buffer), "[%s+%d]", nombreRegistro(o.codigoRegistro), o.desplazamiento);
            else
                snprintf(buffer, sizeof(buffer), "[%s%d]", nombreRegistro(o.codigoRegistro), o.desplazamiento);
            printf("%20s", buffer);
            break;

        case TIPO_NINGUNO:
            // no debería llamarse con este tipo
            break;
    }
}

// Imprime una línea con el formato:
// [dirFisica] XX XX XX ... | MNEM OP_A, OP_B
void mostrarInstruccion(Instruccion instr, Memoria m){
    int i,j=10;
    printf("[%04X] ", instr.dirInicio);

    for (i = 0; i < instr.largoTotal; i++){
        printf("%02X ", m[instr.dirInicio + i]);
    }
    //no me complico tanto para alinearlo:
    j-=i;
    for(i=0;i<j;i++){
        printf("   ");
    }
    printf("| %20s", mnemonico(instr.opcode));

    switch (instr.categoria){
        case DOS_OPERANDOS:
            printf(" ");
            imprimirOperando(instr.operandoA);
            printf(", ");
            imprimirOperando(instr.operandoB);
            break;

        case UN_OPERANDO:
            printf(" ");
            imprimirOperando(instr.operandoA);
            break;

        case SIN_OPERANDOS:
            break;
    }

    printf("\n");
}


char* mnemonico(int opcode) {
    switch(opcode) {
        case OPCODE_SYS: return "SYS";
        case OPCODE_JMP: return "JMP";
        case OPCODE_JP: return "JP";
        case OPCODE_JN: return "JN";
        case OPCODE_JZ: return "JZ";
        case OPCODE_JC: return "JC";
        case OPCODE_JV: return "JV";
        case OPCODE_JNP: return "JNP";
        case OPCODE_JNN: return "JNN";
        case OPCODE_JNZ: return "JNZ";
        case OPCODE_NOT: return "NOT";
        case OPCODE_STOP: return "STOP";
        case OPCODE_MOV: return "MOV";
        case OPCODE_ADD: return "ADD";
        case OPCODE_SUB: return "SUB";
        case OPCODE_MUL: return "MUL";
        case OPCODE_DIV: return "DIV";
        case OPCODE_CMP: return "CMP";
        case OPCODE_AND: return "AND";
        case OPCODE_OR: return "OR";
        case OPCODE_XOR: return "XOR";
        case OPCODE_SWAP: return "SWAP";
        case OPCODE_SHL: return "SHL";
        case OPCODE_SHR: return "SHR";
        case OPCODE_SAR: return "SAR";
        case OPCODE_LDL: return "LDL";
        case OPCODE_LDH: return "LDH";
        case OPCODE_RND: return "RND";
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