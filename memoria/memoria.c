#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memoria.h"
#include "consola.h"
#include "loader.h"
#include "errores.h"

void reservarMemoria(Memoria *memoria, uint32_t tamanioM) {
    *memoria = (uint8_t *)calloc(tamanioM, sizeof(uint8_t));
    if (*memoria == NULL) {
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }
}

void liberarMemoria(Memoria *memoria) {
    if (memoria != NULL && *memoria != NULL) {
        free(*memoria);
        *memoria = NULL;
    }
}

void cargarMemoriaV1(const char *nombreArchivo, Memoria memoria, uint16_t code_size) {
    FILE *archVMX = fopen(nombreArchivo, "rb");

    if (archVMX == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    /// Saltar el encabezado de 8 bytes
    fseek(archVMX, 8, SEEK_SET);

    if (code_size > TAMANIO_MEMORIA) {
        fclose(archVMX);
        reportarError(ERROR_FALLO_DE_SEGMENTO); // El codigo no cabe en la memoria fisica
    }

    if (fread(memoria, 1, code_size, archVMX) != code_size) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO); // Archivo truncado
    }

    fclose(archVMX);
}

void cargarMemoriaV2(const char *nombreArchivo, Memoria memoria, uint32_t tamanoRAM, const tabla_segmentos tablaSegmentos, const Registros registros, const HeaderVMX *header) {
    FILE *archVMX = fopen(nombreArchivo, "rb");

    if (archVMX == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }else{
        /// Saltar el encabezado de 18 bytes de VMX version 2
        if (fseek(archVMX, TAMANIO_HEADER_VMX2, SEEK_SET) != 0) {
            fclose(archVMX);
            reportarError(ERROR_ENCABEZADO_INVALIDO);
        }else{
            /// 1. Cargar byte a byte el Code Segment
            if (registros[REGCS] != 0xFFFFFFFF && header->code_size > 0) {
                int segCode = (int)(registros[REGCS] >> 16);
                uint32_t baseCode = obtenerBaseSegmento(tablaSegmentos, segCode);

                for (uint16_t i = 0; i < header->code_size; i++) {
                    int byteLeido = fgetc(archVMX);
                    if (byteLeido == EOF) {
                        fclose(archVMX);
                        reportarError(ERROR_ENCABEZADO_INVALIDO); // Archivo truncado
                    }
                    uint32_t dirFisica = baseCode + i;
                    if (dirFisica >= tamanoRAM) {
                        fclose(archVMX);
                        reportarError(ERROR_FALLO_DE_SEGMENTO); // Escritura fuera de RAM
                    }
                    memoria[dirFisica] = (uint8_t)byteLeido;
                }
            }

            /// 2. Cargar byte a byte el Const Segment
            if (registros[REGKS] != 0xFFFFFFFF && header->const_size > 0) {
                int segConst = (int)(registros[REGKS] >> 16);
                uint32_t baseConst = obtenerBaseSegmento(tablaSegmentos, segConst);

                for (uint16_t i = 0; i < header->const_size; i++) {
                    int byteLeido = fgetc(archVMX);
                    if (byteLeido == EOF) {
                        fclose(archVMX);
                        reportarError(ERROR_ENCABEZADO_INVALIDO); // Archivo truncado
                    }
                    uint32_t dirFisica = baseConst + i;
                    if (dirFisica >= tamanoRAM) {
                        fclose(archVMX);
                        reportarError(ERROR_FALLO_DE_SEGMENTO); // Escritura fuera de RAM
                    }
                    memoria[dirFisica] = (uint8_t)byteLeido;
                }
            }

            fclose(archVMX);
        }
    }
}

/* se retornan argc y dir lógica de argv. 
    cfg->flagp==0
     *argv=0xFFFFFFFF, *argc=0;
    else 
     *argv=offsetactual (el último antes de cargar el propio argv),*argc=1+cfg->cantParams; 
*/
void cargarParamSegment(const ConfiguracionMV *cfg, Memoria memoria, const tabla_segmentos tablaSegmentos, const Registros registros, uint32_t *argv,uint32_t *argc) {
    *argv=0xFFFFFFFF;
    *argc=0;
    if (registros[REGPS] == 0xFFFFFFFF || !cfg->flagP || cfg->archVMX == NULL) {
        return;
    }else{
        int segParam = (int)(registros[REGPS] >> 16);
        uint32_t baseParam = obtenerBaseSegmento(tablaSegmentos, segParam);

        *argc = 1 + cfg->cantParams;
        uint16_t *offsets = (uint16_t *)malloc(*argc * sizeof(uint16_t));

        if (offsets == NULL) {
            reportarError(ERROR_MEMORIA_INSUFICIENTE);
        }else{
            uint32_t offsetActual = 0;

            /// String 0: nombre del ejecutable .vmx
            offsets[0] = (uint16_t)offsetActual;
            size_t lenVMX = strlen(cfg->archVMX);
            for (size_t i = 0; i <= lenVMX; i++) {
                memoria[baseParam + offsetActual + i] = (uint8_t)cfg->archVMX[i];
            }
            offsetActual += (uint32_t)(lenVMX + 1);

            /// Strings 1..N: parametros del comando
            for (int i = 0; i < cfg->cantParams; i++) {
                offsets[1 + i] = (uint16_t)offsetActual;
                size_t lenP = strlen(cfg->params[i]);
                for (size_t j = 0; j <= lenP; j++) {
                    memoria[baseParam + offsetActual + j] = (uint8_t)cfg->params[i][j];
                }
                offsetActual += (uint32_t)(lenP + 1);
            }
            *argv=registros[REGPS] + offsetActual; //se supone que REGPS==0

            /// Arreglo argv: punteros de 4 bytes en formato big-endian
            /// 16 bits mas significativos = numero de segmento de Param (0x0000)
            /// 16 bits menos significativos = offset del string dentro de Param Segment
            for (int i = 0; i <*argc; i++) {
                uint32_t ptrAddr = baseParam + offsetActual;
                memoria[ptrAddr + 0] = 0x00;
                memoria[ptrAddr + 1] = 0x00;
                memoria[ptrAddr + 2] = (uint8_t)((offsets[i] >> 8) & 0xFF);
                memoria[ptrAddr + 3] = (uint8_t)(offsets[i] & 0xFF);
                offsetActual += 4;
            }

            free(offsets);
        }
    }
}

uint32_t leerDeMemoria(Memoria memoria, uint32_t dirFisica, int cantidadBytes, Registros registros) {
    uint32_t valor = 0;
    for (int k = 0; k < cantidadBytes; k++) {
        valor = (valor << 8) | memoria[dirFisica + k]; // Big-endian
    }
    registros[REGMBR] = valor;
    return valor;
}

void escribirEnMemoria(Memoria memoria, uint32_t dirFisica, uint32_t valor, int cantidadBytes, Registros registros) {
    uint32_t valorAlmacenado = valor;
    if (cantidadBytes == 1) {
        valorAlmacenado = valor & 0xFF;
    } else if (cantidadBytes == 2) {
        valorAlmacenado = valor & 0xFFFF;
    }
    registros[REGMBR] = valorAlmacenado;
    for (int k = 0; k < cantidadBytes; k++) {
        memoria[dirFisica + k] = (valorAlmacenado >> (8 * (cantidadBytes - 1 - k))) & 0xFF; // Big-endian
    }
}

void cargarMemoriaVMI(const char *nombreArchivo, Memoria memoria, uint32_t tamanioRAM) {
    FILE *archVMI = fopen(nombreArchivo, "rb");

    if (archVMI == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    // Saltar header, registros y tabla de segmentos
    if (fseek(archVMI, OFFSET_MEMORIA_VMI, SEEK_SET) != 0) {
        fclose(archVMI);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    size_t leidos = fread(memoria, 1, tamanioRAM, archVMI);
    int sobranBytes = (fgetc(archVMI) != EOF);   // la memoria es lo ultimo del archivo
    fclose(archVMI);

    if (leidos != tamanioRAM || sobranBytes) {
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }
}
