#include "traductor.h"

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