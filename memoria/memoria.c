#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memoria.h"
#include "consola.h"
#include "loader.h"
#include "errores.h"

void reservarMemoria(Memoria *m, uint32_t tamanioM) {
    *m = (uint8_t *)calloc(tamanioM, sizeof(uint8_t));
    if (*m == NULL) {
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }
}

void liberarMemoria(Memoria *m) {
    if (m != NULL && *m != NULL) {
        free(*m);
        *m = NULL;
    }
}

void cargarMemoriaV1(const char *nombreArchivo, Memoria m, uint16_t code_size) {
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

    if (fread(m, 1, code_size, archVMX) != code_size) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO); // Archivo truncado
    }

    fclose(archVMX);
}

void cargarMemoriaV2(const char *nombreArchivo, Memoria m, uint32_t tamanoRAM, const tabla_segmentos t, const Registros r, const HeaderVMX *header) {
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
            if (r[REGCS] != 0xFFFFFFFF && header->code_size > 0) {
                int segCode = (int)(r[REGCS] >> 16);
                uint32_t baseCode = obtenerBaseSegmento(t, segCode);

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
                    m[dirFisica] = (uint8_t)byteLeido;
                }
            }

            /// 2. Cargar byte a byte el Const Segment
            if (r[REGKS] != 0xFFFFFFFF && header->const_size > 0) {
                int segConst = (int)(r[REGKS] >> 16);
                uint32_t baseConst = obtenerBaseSegmento(t, segConst);

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
                    m[dirFisica] = (uint8_t)byteLeido;
                }
            }

            fclose(archVMX);
        }
    }
}

void cargarParamSegment(const ConfiguracionMV *cfg, Memoria m, const tabla_segmentos t, const Registros r) {
    if (r[REGPS] == 0xFFFFFFFF || !cfg->flagP || cfg->archVMX == NULL) {
        return;
    }else{
        int segParam = (int)(r[REGPS] >> 16);
        uint32_t baseParam = obtenerBaseSegmento(t, segParam);

        int argc = 1 + cfg->cantParams;
        uint16_t *offsets = (uint16_t *)malloc(argc * sizeof(uint16_t));

        if (offsets == NULL) {
            reportarError(ERROR_MEMORIA_INSUFICIENTE);
        }else{
            uint32_t offsetActual = 0;

            /// String 0: nombre del ejecutable .vmx
            offsets[0] = (uint16_t)offsetActual;
            size_t lenVMX = strlen(cfg->archVMX);
            for (size_t i = 0; i <= lenVMX; i++) {
                m[baseParam + offsetActual + i] = (uint8_t)cfg->archVMX[i];
            }
            offsetActual += (uint32_t)(lenVMX + 1);

            /// Strings 1..N: parametros del comando
            for (int i = 0; i < cfg->cantParams; i++) {
                offsets[1 + i] = (uint16_t)offsetActual;
                size_t lenP = strlen(cfg->params[i]);
                for (size_t j = 0; j <= lenP; j++) {
                    m[baseParam + offsetActual + j] = (uint8_t)cfg->params[i][j];
                }
                offsetActual += (uint32_t)(lenP + 1);
            }

            /// Arreglo argv: punteros de 4 bytes en formato big-endian
            /// 16 bits mas significativos = numero de segmento de Param (0x0000)
            /// 16 bits menos significativos = offset del string dentro de Param Segment
            for (int i = 0; i < argc; i++) {
                uint32_t ptrAddr = baseParam + offsetActual;
                m[ptrAddr + 0] = 0x00;
                m[ptrAddr + 1] = 0x00;
                m[ptrAddr + 2] = (uint8_t)((offsets[i] >> 8) & 0xFF);
                m[ptrAddr + 3] = (uint8_t)(offsets[i] & 0xFF);
                offsetActual += 4;
            }

            free(offsets);
        }
    }
}

uint32_t leerDeMemoria(Memoria m, uint32_t f, int n, Registros r) {
    uint32_t valor = 0;
    for (int k = 0; k < n; k++) {
        valor = (valor << 8) | m[f + k]; // Big-endian
    }
    r[REGMBR] = valor;
    return valor;
}

void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, int n, Registros r) {
    r[REGMBR] = valor;
    for (int k = 0; k < n; k++) {
        m[f + k] = (valor >> (8 * (n - 1 - k))) & 0xFF; // Big-endian
    }
}
