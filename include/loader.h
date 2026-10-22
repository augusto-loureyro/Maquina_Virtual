#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

/**
 * Lee y valida el encabezado de un ejecutable VMX.
 *
 * Verifica:
 *   - Identificador "VMX26" (bytes 0-4)
 *   - Versión 1 (byte 5)
 *   - Obtiene el tamaño del segmento de código en bytes (bytes 6-7, big-endian)
 *
 */
void procesarHeader(const char *nombreArchivo, uint8_t *version, uint16_t *code_size);

#endif // LOADER_H
