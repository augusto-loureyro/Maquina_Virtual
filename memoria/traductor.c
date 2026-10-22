#include "traductor.h"

dirFisica traducir(dirLogica l, tabla_segmentos t, Registros r, int cantidadBytes, int esAccesoAMemoria) {
    int segmento = l >> 16;
    uint16_t desplazamiento = l & 0xFFFF;

    /// Validacion de segmento existente en la tabla
    if (segmento >= CANT_SEG || obtenerBaseSegmento(t, segmento) == 0xFFFF) {
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }

    dirFisica fisica = (dirFisica)obtenerBaseSegmento(t, segmento) + desplazamiento;

    /// Si es un acceso a memoria para leer/escribir datos (no fetch de instruccion):
    /// Se valida el limite del segmento y se actualizan los registros LAR y MAR
    if (esAccesoAMemoria) {
        if ((uint32_t)desplazamiento + cantidadBytes > obtenerTamanioSegmento(t, segmento)) {
            reportarError(ERROR_FALLO_DE_SEGMENTO);
        }

        r[REGLAR] = l;
        r[REGMAR] = ((uint32_t)cantidadBytes << 16) | (fisica & 0xFFFF);
    }

    return fisica;
}
