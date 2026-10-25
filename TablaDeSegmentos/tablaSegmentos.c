#include "tablaSegmentos.h"


void inicializarTablaSegmentos(tabla_segmentos t){
    int i;

    for(i=0;i<CANT_SEG;i++){
        t[i]=0xFFFFFFFF;
    }
}
/*
Todos los segmentos deben cargarse de forma contigua en la memoria, 
respetando el siguiente orden: Param Segment, Const Segment, Code 
Segment, Data Segment, Extra Segment, Stack Segment. Sin embargo, a 
excepción del Code Segment y del Stack Segment, puede suceder que no 
todos los segmentos estén presentes. 

La tabla de descriptores de segmentos se mantiene igual que en la primera parte. Sin embargo, ahora 
solo deben crearse entradas para los segmentos que tengan un tamaño mayor a cero. Esto quiere decir 
que la posición de cada segmento en la tabla no será siempre la misma. La construcción de la tabla debe 
ser posterior a la creación de los segmentos y el orden en la tabla debe ser el mismo que su ubicación 
física en la memoria principal.
*/
void cargarTablaSegmentos(tabla_segmentos t,IndiceSgm *index, Header hdr){
    uint16_t offset=0, numI=0;

    inicializarTablaSegmentos(t);

    if(hdr.sizePS>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.sizePS;
        offset+=hdr.sizePS;
        index->ps=numI++;
    }
    if(hdr.sizeKS>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.sizeKS;
        offset+=hdr.sizeKS;
        index->ks=numI++;
    }
    if(hdr.sizeCS>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.sizeCS;
        offset+=hdr.sizeCS;
        index->cs=numI++;
    }
    if(hdr.sizeDS>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.sizeDS;
        offset+=hdr.sizeDS;
        index->ds=numI++;
    }
    if(hdr.sizeES>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.sizeES;
        offset+=hdr.sizeES;
        index->es=numI++;
    }
    if(hdr.sizeSS>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.sizeSS;
        offset+=hdr.sizeSS;
        index->ss=numI++;
    }
}

//estos van a seguir funcionando igual, debo pasar segmento.tal
uint16_t obtenerBaseSegmento(tabla_segmentos t,int segmento) {
    return (uint16_t)(t[segmento] >> 16);
}

uint16_t obtenerTamanioSegmento(tabla_segmentos t,int segmento) {
    return (uint16_t)(t[segmento] & 0xFFFF);
}
