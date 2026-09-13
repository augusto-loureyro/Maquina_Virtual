#ifndef ERRORES_H
#define ERRORES_H

#include<stdio.h>
#include <stdlib.h>

typedef enum {
    ERROR_ARGUMENTOS_INVALIDOS,
    ERROR_ARCHIVO_NO_ENCONTRADO,
    ERROR_ENCABEZADO_INVALIDO,
    ERROR_VERSION_NO_SOPORTADA,
    ERROR_INSTRUCCION_INVALIDA,
    ERROR_DIVISION_POR_CERO,
    ERROR_FALLO_DE_SEGMENTO
} TipoError;

void reportarError(TipoError tipo);
#endif