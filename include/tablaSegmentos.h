#ifndef TABLASEGMENTOS_H
#define TABLASEGMENTOS_H

#define CANT_SEG 8
#include<stdint.h>
#include "loader.h"
#include "IndiceSegmentos.h" // un struct segmento con variables ps,cs,... para el índice

typedef uint32_t tabla_segmentos[CANT_SEG];

void incializarTablaSegmentos(tabla_segmentos t);
void cargarTablaSegmentos(tabla_segmentos t,IndiceSgm *index,Header hdr);
uint16_t obtenerBaseSegmento(tabla_segmentos t,int segmento);
uint16_t obtenerTamanioSegmento(tabla_segmentos t,int segmento);

#endif