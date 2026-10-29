#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "cpu.h"
#include "memoria.h"

void mostrarInstruccion(Instruccion instr, Memoria memoria, int esEntryPoint);
const char* mnemonico(int opcode);
const char* nombreRegistro(int reg);
void mostrarConstantes(Memoria memoria, tabla_segmentos tabla, Registros registros);
void mostrarDisassembly(Memoria memoria, tabla_segmentos tabla, Registros registros);

#endif // DISASSEMBLER_H
