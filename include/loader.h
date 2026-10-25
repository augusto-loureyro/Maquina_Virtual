#ifndef LOADER_H
#define LOADER_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "errores.h"

typedef struct {
    uint8_t version;
    uint16_t sizeCS,sizeDS,sizeES,sizeSS,sizeKS,offsetEP,sizePS; //agrego sizePS, aunque no se lee de hdr...
}Header;

//procesarHeader valida identificador, version y obtiene code_size
void procesarHeaderVMX(const char *nombreArchivo, Header *hdr);

void leerCodigo();

#endif