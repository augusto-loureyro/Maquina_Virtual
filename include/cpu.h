#ifndef CPU_H
#define CPU_H

#include "decodificador.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "traductor.h"
#include "alu.h"

#define NMASK (1u << CC_BIT_N)
#define ZMASK (1u << CC_BIT_Z)
#define CMASK (1u << CC_BIT_C)
#define VMASK (1u << CC_BIT_V)

typedef struct {
    int opcode;
    int categoria;
    dirFisica dirInicio;
    int largoTotal;
    Operando operandoA;
    Operando operandoB;
} Instruccion;

uint32_t armarRegistroOperando(int tipo, uint8_t *bytes);
Instruccion buscarInstruccion(tabla_segmentos t, Memoria m, Registros r);
uint32_t leerValorOperando(Operando o, tabla_segmentos t, Memoria m, Registros r);
void escribirValorOperando(Operando o, uint32_t valor, tabla_segmentos t, Memoria m, Registros r);
int hayMasInstrucciones(tabla_segmentos t, Registros r);
void saltarA(uint32_t desplazamiento, Registros r);
void ejecutarInstruccion(Instruccion instr, tabla_segmentos t, Memoria m, Registros r);
void ejecutarPrograma(tabla_segmentos t, Memoria m, Registros r,int flagD);
uint32_t LDH(uint32_t b, uint16_t h);
uint32_t LDL(uint32_t b, uint16_t l);

#endif

/*
caary, explicacion profe:

   1-> hago la cuenta normal, en 32 bit
   2 -> hago la cuenta en 64 bit. 

   4bit _ 7+1 -> 0001 + 0111 = 1000 -> -8 en vez de 8
   8bit _ 7+1 -> 00000001 + 00000111 = 000000001000 -> 8 
       carry si el res de 8bit > 1111 
       
    overflow
       hago la cuenta con signo, en 32 y 64. si dio distinto, hubo overflow. 

    ------
    para la resta tener en cuenta que se hace suma, comp 2 ...      
*/