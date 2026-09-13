#include "include/traductor.h"

/* anterior
dirFisica traducir(dirLogica l, tabla_segmentos t, int cantidadBytes, int verificarLimite){
    int segmento = l >> 16;
    uint16_t desplazamiento = l & 0xFFFF;

    if (segmento >= CANT_SEG || obtenerTamanioSegmento(t, segmento) == 0xFFFF){
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }

    dirFisica fisica = obtenerBaseSegmento(t, segmento) + desplazamiento;

    if (verificarLimite && (desplazamiento + cantidadBytes) > obtenerTamanioSegmento(t, segmento)){
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }

    return fisica;
}
*/
dirFisica traducir(dirLogica l, tabla_segmentos t, Registros r, int cantidadBytes, int esAccesoAMemoria){
    int segmento = l >> 16;
    uint16_t desplazamiento = l & 0xFFFF;

    if (segmento >= CANT_SEG || segmento < 0 || obtenerBaseSegmento(t, segmento) == 0xFFFF){
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