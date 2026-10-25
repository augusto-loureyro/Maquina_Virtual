#include <stdio.h>
#include <string.h>

#include "include/loader.h"
#include "include/errores.h"

static uint16_t leerUint16BigEndian(const uint8_t *bytes) {
    return (uint16_t)(((uint16_t)bytes[0] << 8) | bytes[1]);
}

void procesarHeader(const char *nombreArchivo, HeaderVMX *header) {
    FILE *archivo;
    uint8_t buffer[TAMANIO_HEADER_VMX2];
    size_t tamanioHeader;

    if (nombreArchivo == NULL || header == NULL) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    memset(header, 0, sizeof(*header));
    archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    if (fread(buffer, 1, 6, archivo) != 6 ||
        memcmp(buffer, IDENTIFICADOR_VMX, 5) != 0) {
        fclose(archivo);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    header->version = buffer[5];
    if (header->version == 1) {
        tamanioHeader = TAMANIO_HEADER_VMX1;
    } else if (header->version == 2) {
        tamanioHeader = TAMANIO_HEADER_VMX2;
    } else {
        fclose(archivo);
        reportarError(ERROR_VERSION_NO_SOPORTADA);
        return;
    }

    if (fread(buffer + 6, 1, tamanioHeader - 6, archivo) != tamanioHeader - 6) {
        fclose(archivo);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }
    fclose(archivo);

    header->code_size = leerUint16BigEndian(buffer + 6);
    if (header->version == 2) {
        header->data_size = leerUint16BigEndian(buffer + 8);
        header->extra_size = leerUint16BigEndian(buffer + 10);
        header->stack_size = leerUint16BigEndian(buffer + 12);
        header->const_size = leerUint16BigEndian(buffer + 14);
        header->entry_point = leerUint16BigEndian(buffer + 16);
    }
}

void mostrarHeader(const HeaderVMX *header) {
    printf("Encabezado VMX (Version %u):\n", header->version);
    printf(" Identificador : %s\n", IDENTIFICADOR_VMX);
    printf(" Code Segment  : %u bytes\n", header->code_size);
    if (header->version == 2) {
        printf(" Data Segment  : %u bytes\n", header->data_size);
        printf(" Extra Segment : %u bytes\n", header->extra_size);
        printf(" Stack Segment : %u bytes\n", header->stack_size);
        printf(" Const Segment : %u bytes\n", header->const_size);
        printf(" Entry Point   : 0x%04X (%u)\n",
               header->entry_point, header->entry_point);
    }
}
