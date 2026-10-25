#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

#define IDENTIFICADOR_VMX "VMX26"
#define TAMANIO_HEADER_VMX1 8
#define TAMANIO_HEADER_VMX2 18

typedef struct {
    uint8_t version;         /** Version del archivo binario (1 o 2) */
    uint16_t code_size;      /** Tamaño del Code Segment en bytes */
    uint16_t data_size;      /** Tamaño del Data Segment en bytes */
    uint16_t extra_size;     /** Tamaño del Extra Segment en bytes */
    uint16_t stack_size;     /** Tamaño del Stack Segment en bytes */
    uint16_t const_size;     /** Tamaño del Const Segment en bytes  */
    uint16_t entry_point;    /** Offset del entry point en bytes */
    uint16_t param_size;     /** Tamaño del Param Segment creado en tiempo de ejecucion */
} HeaderVMX;

void procesarHeader(const char *nombreArchivo, HeaderVMX *header);
void mostrarHeader(const HeaderVMX *header);

#endif