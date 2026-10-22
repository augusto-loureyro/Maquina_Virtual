#include <stdio.h>
#include <string.h>
#include "loader.h"
#include "errores.h"

void procesarHeader(const char *nombreArchivo, uint8_t *version, uint16_t *code_size) {
    FILE *archVMX = fopen(nombreArchivo, "rb");
    uint8_t header[8];

    if (archVMX == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    if (fread(header, 1, 8, archVMX) != 8) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    /// Bytes 0-4: Identificador "VMX26"
    if (memcmp(header, "VMX26", 5) != 0) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    /// Byte 5: Version del formato (debe ser 1)
    *version = header[5];
    if (*version != 1) {
        fclose(archVMX);
        reportarError(ERROR_VERSION_NO_SOPORTADA);
    }

    /// Bytes 6-7: Tamaño del segmento de codigo (big-endian)
    *code_size = (header[6] << 8) | header[7];

    fclose(archVMX);
}
