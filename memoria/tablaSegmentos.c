#include <stdio.h>
#include <string.h>
#include "tablaSegmentos.h"
#include "memoria.h"
#include "errores.h"

void inicializarTablaSegmentos(tabla_segmentos tablaSegmentos) {
    for (int i = 0; i < CANT_SEG; i++) {
        tablaSegmentos[i] = 0xFFFFFFFF; // Entradas no utilizadas inicializadas en -1
    }
}

void cargarTablaSegmentosV1(tabla_segmentos tablaSegmentos, uint16_t code_size) {
    inicializarTablaSegmentos(tablaSegmentos);

    /// Segmento 0: Codigo (comienza en 0, tamano = code_size)
    uint16_t baseCode = 0;
    uint16_t tamanioCode = code_size;

    /// Segmento 1: Datos (comienza inmediatamente despues del codigo, ocupa el resto)
    uint16_t baseData = code_size;
    uint16_t tamanioData = TAMANIO_MEMORIA - code_size;

    tablaSegmentos[CODE] = ((uint32_t)baseCode << 16) | tamanioCode;
    tablaSegmentos[DATA] = ((uint32_t)baseData << 16) | tamanioData;
}

void cargarTablaSegmentosV2(tabla_segmentos tablaSegmentos, const ConfiguracionMV *cfg) {
    uint16_t param_size = calcularTamanioParamSegment(cfg);
    uint16_t const_size = cfg->header.const_size;
    uint16_t code_size  = cfg->header.code_size;
    uint16_t data_size  = cfg->header.data_size;
    uint16_t extra_size = cfg->header.extra_size;
    uint16_t stack_size = cfg->header.stack_size;

    /// Validacion de requerimientos esenciales: Code y Stack deben existir
    if (code_size == 0 || stack_size == 0) {
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    uint32_t totalProceso = (uint32_t)param_size + const_size + code_size + data_size + extra_size + stack_size;
    uint32_t totalRAM = cfg->mem_kib * 1024;

    /// Comprobacion de memoria suficiente antes de proceder a la carga
    if (totalProceso > totalRAM) {
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }else{
        inicializarTablaSegmentos(tablaSegmentos);

        /// Ubicacion fisica contigua de segmentos en memoria:
        /// Param -> Const -> Code -> Data -> Extra -> Stack
        /// Solo se generan entradas en la tabla para segmentos con tamano > 0
        uint32_t baseActual = 0;
        int idx = 0;

        /// 1. Param Segment
        if (param_size > 0) {
            tablaSegmentos[idx++] = (baseActual << 16) | param_size;
            baseActual += param_size;
        }

        /// 2. Const Segment
        if (const_size > 0) {
            tablaSegmentos[idx++] = (baseActual << 16) | const_size;
            baseActual += const_size;
        }

        /// 3. Code Segment
        if (code_size > 0) {
            tablaSegmentos[idx++] = (baseActual << 16) | code_size;
            baseActual += code_size;
        }

        /// 4. Data Segment
        if (data_size > 0) {
            tablaSegmentos[idx++] = (baseActual << 16) | data_size;
            baseActual += data_size;
        }

        /// 5. Extra Segment
        if (extra_size > 0) {
            tablaSegmentos[idx++] = (baseActual << 16) | extra_size;
            baseActual += extra_size;
        }

        /// 6. Stack Segment
        if (stack_size > 0) {
            tablaSegmentos[idx++] = (baseActual << 16) | stack_size;
            baseActual += stack_size;
        }
    }
}

uint16_t obtenerBaseSegmento(const tabla_segmentos tablaSegmentos, int segmento) {
    return (uint16_t)(tablaSegmentos[segmento] >> 16);
}

uint16_t obtenerTamanioSegmento(const tabla_segmentos tablaSegmentos, int segmento) {
    return (uint16_t)(tablaSegmentos[segmento] & 0xFFFF);
}

int esSegmentoValido(const tabla_segmentos tablaSegmentos, int segmento) {
    if (segmento < 0 || segmento >= CANT_SEG) {
        return 0;
    }
    return tablaSegmentos[segmento] != 0xFFFFFFFF;
}

uint16_t calcularTamanioParamSegment(const ConfiguracionMV *cfg) {
    if (!cfg->flagP || cfg->archVMX == NULL) {
        return 0;
    }

    uint32_t tam = (uint32_t)(strlen(cfg->archVMX) + 1);
    for (int i = 0; i < cfg->cantParams; i++) {
        tam += (uint32_t)(strlen(cfg->params[i]) + 1);
    }
    // Arreglo argv: (1 + cantParams) punteros de 4 bytes
    tam += (uint32_t)(1 + cfg->cantParams) * sizeof(uint32_t);

    return (uint16_t)tam;
}

void mostrarTablaSegmentos(const tabla_segmentos tablaSegmentos) {
    printf("Tabla de Descriptores de Segmentos (8 entradas):\n");
    for (int i = 0; i < CANT_SEG; i++) {
        if (tablaSegmentos[i] != 0xFFFFFFFF) {
            printf("  Entrada %d: Base = 0x%04X (%5u) | Limite = 0x%04X (%5u bytes)\n",
                   i, obtenerBaseSegmento(tablaSegmentos, i), obtenerBaseSegmento(tablaSegmentos, i),
                   obtenerTamanioSegmento(tablaSegmentos, i), obtenerTamanioSegmento(tablaSegmentos, i));
        } else {
            printf("  Entrada %d: 0xFFFFFFFF (vacia)\n", i);
        }
    }
}

