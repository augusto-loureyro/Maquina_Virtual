#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "cpu.h"      // Instruccion, Operando
#include "memoria.h"  // Memoria

void mostrarInstruccion(Instruccion instr, Memoria m);
void imprimirOperando(Operando o);
char* mnemonico(int opcode);
char* nombreRegistro(int reg);

#endif