#include "../include/traductor.h"

//cuando se traduce para acceso a memoria:
// Cada vez que se realiza una operación en la memoria, se debe cargar en el registro 
// LAR -> la dirección lógica a la que se quiere acceder 
// MAR ->  la cantidad de bytes en la parte alta del registro; Luego de realizar la traducción a una dirección física, el resultado debe almacenarse en la parte baja del registro
dirFisica traducir(dirLogica l, tabla_segmentos t, Registros r, int cantidadBytes, int esAccesoAMemoria){
    int segmento = l >> 16; // || segmento < 0 quitado, nunca puede dar neg.
    uint16_t desplazamiento = l & 0xFFFF;

    if (segmento >= CANT_SEG || obtenerBaseSegmento(t, segmento) == 0xFFFF){
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }

    dirFisica fisica = obtenerBaseSegmento(t, segmento) + desplazamiento;

    if (esAccesoAMemoria){
        if ((desplazamiento + cantidadBytes) > obtenerTamanioSegmento(t, segmento)){
            reportarError(ERROR_FALLO_DE_SEGMENTO);
        }

        r[REGLAR] = l;
        r[REGMAR] = ((uint32_t)cantidadBytes << 16) | (fisica & 0xFFFF);
    }

    return fisica;
}