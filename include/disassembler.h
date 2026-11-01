#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "cpu.h"
#include "memoria.h"
#include "consola.h"

void mostrarInstruccion(Instruccion instr, Memoria memoria, int esEntryPoint, int esV2);
const char* mnemonico(int opcode);
const char* nombreRegistro(int reg);
void mostrarConstantes(Memoria memoria, tabla_segmentos tabla, Registros registros);
void mostrarDisassembly(Memoria memoria, tabla_segmentos tabla, Registros registros, const ConfiguracionMV *cfg);

#endif // DISASSEMBLER_H
