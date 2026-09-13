#ifndef LOADER_H
#define LOADER_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "errores.h"

//procesarHeader valida identificador, version y obtiene code_size
void procesarHeader(const char *nombreArchivo, uint8_t *version, uint16_t *code_size);

void leerCodigo();

#endif