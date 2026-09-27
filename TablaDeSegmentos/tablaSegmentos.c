#include "../include/tablaSegmentos.h"
#include "../include/memoria.h"


void inicializarTablaSegmentos(tabla_segmentos t){
    int i;

    for(i=0;i<CANT_SEG;i++){
        t[i]=0xFFFFFFFF;
    }
}
void cargarTablaSegmentos(tabla_segmentos t,uint16_t code_size){
    uint16_t baseCode, tamanioCode;
    uint16_t baseData, tamanioData;

    inicializarTablaSegmentos(t);

    baseCode    = 0;
    tamanioCode = code_size;

    baseData    = code_size;
    tamanioData = TAMANIO_MEMORIA - code_size;

    t[CODE] = ((uint32_t)baseCode << 16) | tamanioCode;
    t[DATA] = ((uint32_t)baseData << 16) | tamanioData;
}

uint16_t obtenerBaseSegmento(tabla_segmentos t,int segmento) {
    return (uint16_t)(t[segmento] >> 16);
}

uint16_t obtenerTamanioSegmento(tabla_segmentos t,int segmento) {
    return (uint16_t)(t[segmento] & 0xFFFF);
}
