#include "tablaSegmentos.h"
#include "memoria.h"

void inicializarTablaSegmentos(tabla_segmentos t) {
    for (int i = 0; i < CANT_SEG; i++) {
        t[i] = 0xFFFFFFFF; // Entradas no utilizadas inicializadas en -1
    }
}

void cargarTablaSegmentos(tabla_segmentos t, uint16_t code_size) {
    inicializarTablaSegmentos(t);

    /// Segmento 0: Codigo (comienza en 0, tamaño = code_size)
    uint16_t baseCode = 0;
    uint16_t tamanioCode = code_size;

    /// Segmento 1: Datos (comienza inmediatamente despues del codigo, ocupa el resto)
    uint16_t baseData = code_size;
    uint16_t tamanioData = TAMANIO_MEMORIA - code_size;

    t[CODE] = ((uint32_t)baseCode << 16) | tamanioCode;
    t[DATA] = ((uint32_t)baseData << 16) | tamanioData;
}

uint16_t obtenerBaseSegmento(tabla_segmentos t, int segmento) {
    return (uint16_t)(t[segmento] >> 16);
}

uint16_t obtenerTamanioSegmento(tabla_segmentos t, int segmento) {
    return (uint16_t)(t[segmento] & 0xFFFF);
}
