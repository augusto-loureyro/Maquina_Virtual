#include "traductor.h"

dirFisica traducir(dirLogica direccionLogica, tabla_segmentos tablaSegmentos, Registros registros, int cantidadBytes, int esAccesoAMemoria) {
    int segmento = direccionLogica >> 16;
    uint16_t desplazamiento = direccionLogica & 0xFFFF;

    /// Validacion de segmento existente en la tabla
    if (!esSegmentoValido(tablaSegmentos, segmento)) {
        reportarError(ERROR_FALLO_DE_SEGMENTO);
    }

    dirFisica fisica = (dirFisica)obtenerBaseSegmento(tablaSegmentos, segmento) + desplazamiento;

    /// Si es un acceso a memoria para leer/escribir datos (no fetch de instruccion):
    /// Se valida el limite del segmento y se actualizan los registros LAR y MAR
    if (esAccesoAMemoria) {
        if ((uint32_t)desplazamiento + cantidadBytes > obtenerTamanioSegmento(tablaSegmentos, segmento)) {
            reportarError(ERROR_FALLO_DE_SEGMENTO);
        }

        registros[REGLAR] = direccionLogica;
        registros[REGMAR] = ((uint32_t)cantidadBytes << 16) | (fisica & 0xFFFF);
    }

    return fisica;
}
