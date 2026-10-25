#include <stdio.h>
#include <string.h>
#include "loader.h"
#include "errores.h"

void procesarHeader(const char *nombreArchivo, HeaderVMX *header) {
    FILE *archVMX = fopen(nombreArchivo, "rb");
    uint8_t buffer[TAMANIO_HEADER_VMX2];

    if (archVMX == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    /// Leer los primeros 6 bytes para verificar identificador y version
    if (fread(buffer, 1, 6, archVMX) != 6) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    /// Bytes 0-4: Identificador "VMX26"
    if (memcmp(buffer, IDENTIFICADOR_VMX, 5) != 0) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    /// Byte 5: Version del formato
    header->version = buffer[5];

    if (header->version == 1) {
        /// Version 1: Header de 8 bytes
        if (fread(&buffer[6], 1, 2, archVMX) != 2) {
            fclose(archVMX);
            reportarError(ERROR_ENCABEZADO_INVALIDO);
        }
        header->code_size   = (uint16_t)((buffer[6] << 8) | buffer[7]);
        header->data_size   = 0;
        header->extra_size  = 0;
        header->stack_size  = 0;
        header->const_size  = 0;
        header->entry_point = 0;
    } else if (header->version == 2) {
        /// Version 2: Header de 18 bytes
        if (fread(&buffer[6], 1, 12, archVMX) != 12) {
            fclose(archVMX);
            reportarError(ERROR_ENCABEZADO_INVALIDO);
        }
        header->code_size   = (uint16_t)((buffer[6]  << 8) | buffer[7]);
        header->data_size   = (uint16_t)((buffer[8]  << 8) | buffer[9]);
        header->extra_size  = (uint16_t)((buffer[10] << 8) | buffer[11]);
        header->stack_size  = (uint16_t)((buffer[12] << 8) | buffer[13]);
        header->const_size  = (uint16_t)((buffer[14] << 8) | buffer[15]);
        header->entry_point = (uint16_t)((buffer[16] << 8) | buffer[17]);
    } else {
        fclose(archVMX);
        reportarError(ERROR_VERSION_NO_SOPORTADA);
    }

    fclose(archVMX);
}

void mostrarHeader(const HeaderVMX *header) {
    printf("Encabezado VMX (Version %u):\n", header->version);
    printf(" Identificador : %s\n", IDENTIFICADOR_VMX);
    printf(" Version       : %u\n", header->version);
    printf(" Code Segment  : %u bytes\n", header->code_size);
    if (header->version >= 2) {
        printf(" Data Segment  : %u bytes\n", header->data_size);
        printf(" Extra Segment : %u bytes\n", header->extra_size);
        printf(" Stack Segment : %u bytes\n", header->stack_size);
        printf(" Const Segment : %u bytes\n", header->const_size);
        printf(" Entry Point   : 0x%04X (%u)\n", header->entry_point, header->entry_point);
    }
}
