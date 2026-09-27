// básico: Abre el archivo binario y lee la cabecera, comprobando identificador, version, y obteniendo code_size.

#include "include/loader.h"

void procesarHeader(const char *nombreArchivo, uint8_t *version, uint16_t *code_size){
    FILE *archVMX;
    uint8_t header[8];

    if((archVMX = fopen(nombreArchivo, "rb")) == NULL){
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    if(fread(header, 1, 8, archVMX) != 8){
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    if(memcmp(header, "VMX26", 5) != 0){
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    *version = header[5];
    if(*version != 1){
        fclose(archVMX);
        reportarError(ERROR_VERSION_NO_SOPORTADA);
    }

    *code_size = (header[6] << 8) | header[7];
    fclose(archVMX);
}