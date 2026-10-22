#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "cpu.h"
#include "memoria.h"

void mostrarInstruccion(Instruccion instr, Memoria m);
const char* mnemonico(int opcode);
const char* nombreRegistro(int reg);

#endif // DISASSEMBLER_H
