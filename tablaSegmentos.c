#include "tablaSegmentos.h"
#include"memoria.h"
#include "util.h"


void inicializarTablaSegmentos(tabla_segmentos t){
    int i;

    for(i=0;i<CANT_SEG;i++){
        t[i]=0xFFFFFFFF;
    }
}
void cargarTablaSegmentos(tabla_segmentos t,uint16_t code_size){
    inicializarTablaSegmentos(t);

    t[CODE]=LCH(t[CODE],0);
    t[CODE]=LCL(t[CODE],code_size);

    t[DATA]=LCH(t[DATA],code_size);
    t[DATA]=LCL(t[DATA],TAMANIO_MEMORIA-code_size);
}

uint16_t obtenerBaseSegmento(tabla_segmentos t,int segmento) {
    return (uint16_t)(t[segmento] >> 16);
}

uint16_t obtenerTamanioSegmento(tabla_segmentos t,int segmento) {
    return (uint16_t)(t[segmento] & 0xFFFF);
}
// LCL y LCH, métodos para cargar parte baja o alta sin tocar lo que ya estaba. 