#ifndef TABLASEGMENTOS_H
#define TABLASEGMENTOS_H

#define CANT_SEG 8
#define CODE 0 
#define DATA 1
#include<stdint.h>
#include "util.h"

typedef uint32_t tabla_segmentos[CANT_SEG];

void incializarTablaSegmentos(tabla_segmentos t);
void cargarTablaSegmentos(tabla_segmentos t,uint16_t code_size);
uint16_t obtenerBaseSegmento(tabla_segmentos t,int segmento);
uint16_t obtenerTamanioSegmento(tabla_segmentos t,int segmento);

#endif 