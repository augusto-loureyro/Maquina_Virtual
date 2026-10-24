#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

#define IDENTIFICADOR_VMX "VMX26"
#define TAMANIO_HEADER_VMX1 8
#define TAMANIO_HEADER_VMX2 18

/**
 * Estructura que almacena los metadatos del encabezado de un archivo VMX.
 */
typedef struct {
    uint8_t version;         /** Version del archivo binario (1 o 2) */
    uint16_t code_size;      /** Tamaño del Code Segment en bytes */
    uint16_t data_size;      /** Tamaño del Data Segment en bytes */
    uint16_t extra_size;     /** Tamaño del Extra Segment en bytes */
    uint16_t stack_size;     /** Tamaño del Stack Segment en bytes */
    uint16_t const_size;     /** Tamaño del Const Segment en bytes  */
    uint16_t entry_point;    /** Offset del entry point en bytes */
} HeaderVMX;

/**
 * Lee y valida el encabezado de un ejecutable VMX (version 1 o version 2).
 *
 * Para Version 1 (encabezado de 8 bytes):
 *   - Bytes 0-4: "VMX26"
 *   - Byte 5: 1
 *   - Bytes 6-7: code_size
 *   - Resto de campos inicializados en 0.
 *
 * Para Version 2 (encabezado de 18 bytes):
 *   - Bytes 0-4: "VMX26"
 *   - Byte 5: 2
 *   - Bytes 6-7: code_size
 *   - Bytes 8-9: data_size
 *   - Bytes 10-11: extra_size
 *   - Bytes 12-13: stack_size
 *   - Bytes 14-15: const_size
 *   - Bytes 16-17: entry_point
 */
void procesarHeader(const char *nombreArchivo, HeaderVMX *header);

/**
 * Muestra por pantalla el encabezado.
 */
void mostrarHeader(const HeaderVMX *header);

#endif // LOADER_H
