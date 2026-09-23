#ifndef INSTRUCCION_H_INCLUDED
#define INSTRUCCION_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "maquinaVirtual.h"
#include "traductor.h"
#include "memoriaPrincipal.h"

#define SYS 0x00
#define JMP 0x01
#define JP 0x02
#define JN 0x03
#define JZ 0x04
#define JC 0x05
#define JV 0x06
#define JNP 0x07
#define JNN 0x08
#define JNZ 0x09
#define NOT 0x0A
#define STOP 0x0F
#define MOV 0x10
#define ADD 0x11
#define SUB 0x12
#define MUL 0x13
#define DIV 0x14
#define CMP 0x15
#define AND 0x16
#define OR 0x17
#define XOR 0x18
#define SWAP 0x19
#define SHL 0x1A
#define SHR 0x1B
#define SAR 0x1C
#define LDL 0x1D
#define LDH 0x1E
#define RND 0x1F


void ejecutarInstruccion(ETMaquinaVirtual *maqVirt);
bool verificarOverflow(int valA, int valB, int resultado, char op);

#endif // INSTRUCCION_H_INCLUDED
