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

/* header VMI
        0 a 4 -> id
        5 -> v
        6 a 7 -> mem_kib
*/

void procesarHeaderVMI(const char *nombreArchivo, HeaderVMI *header) {
    FILE *archVMI = fopen(nombreArchivo, "rb");
    uint8_t buffer[TAMANIO_HEADER_VMI];

    if (archVMI == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    /// Leer el header completo (8 bytes) y cerrar el archivo
    size_t leidos = fread(buffer, 1, TAMANIO_HEADER_VMI, archVMI);
    fclose(archVMI);

    /// Header incompleto o identificador incorrecto ("VMI26")
    if (leidos != TAMANIO_HEADER_VMI || memcmp(buffer, IDENTIFICADOR_VMI, 5) != 0) {
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    /// Byte 5: Version del formato
    header->version = buffer[5];
    if (header->version != 1) {
        reportarError(ERROR_VERSION_NO_SOPORTADA);
    }

    /// Bytes 6-7: Tamaño de la memoria en KiB (big endian)
    header->mem_kib = (uint16_t)((buffer[6] << 8) | buffer[7]);
}

/* 
    Estructura del .vmi:
        header (8) | registros (128) | tabla de segmentos (32) | memoria (tamanioRAM)
        Lee los bytes del cuerpo (tabla y registros, memoria no); el header ya fue validado por procesarHeaderVMI. 
*/
void leerRegistrosYTablaVMI(const char *nombreArchivo,uint8_t bytesRegistros[BYTES_REGISTROS_VMI],uint8_t bytesTabla[BYTES_TABLA_VMI]) {
    FILE *archVMI = fopen(nombreArchivo, "rb");
    if (archVMI == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    /// Saltear el header
    fseek(archVMI, TAMANIO_HEADER_VMI, SEEK_SET);

    size_t leidosRegistros = fread(bytesRegistros, 1, BYTES_REGISTROS_VMI, archVMI);
    size_t leidosTabla     = fread(bytesTabla, 1, BYTES_TABLA_VMI, archVMI);

    fclose(archVMI);

    if (leidosRegistros != BYTES_REGISTROS_VMI || leidosTabla != BYTES_TABLA_VMI) {
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }
}

// por ahora, acá
/** Convierte 4 bytes en big endian a un uint32_t */
uint32_t bytesAUint32BE(const uint8_t *b) {
    return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
           ((uint32_t)b[2] << 8)  |  (uint32_t)b[3];
}