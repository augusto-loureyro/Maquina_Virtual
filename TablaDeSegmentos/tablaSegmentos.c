#include "tablaSegmentos.h"
#include "errores.h"


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
void cargarTablaSegmentos(tabla_segmentos t,IndiceSgm *index, HeaderVMX hdr){
    uint16_t offset=0, numI=0;

    if (t == 0 || index == 0) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    inicializarTablaSegmentos(t);
    index->ps = -1;
    index->cs = -1;
    index->ds = -1;
    index->es = -1;
    index->ss = -1;
    index->ks = -1;

    if(hdr.param_size>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.param_size;
        offset+=hdr.param_size;
        index->ps=numI++;
    }
    if(hdr.const_size>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.const_size;
        offset+=hdr.const_size;
        index->ks=numI++;
    }
    if(hdr.code_size>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.code_size;
        offset+=hdr.code_size;
        index->cs=numI++;
    }
    if(hdr.data_size>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.data_size;
        offset+=hdr.data_size;
        index->ds=numI++;
    }
    if(hdr.extra_size>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.extra_size;
        offset+=hdr.extra_size;
        index->es=numI++;
    }
    if(hdr.stack_size>0){
        t[numI]= ((uint32_t)offset << 16) | hdr.stack_size;
        offset+=hdr.stack_size;
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
