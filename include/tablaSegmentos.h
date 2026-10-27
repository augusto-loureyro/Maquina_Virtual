#ifndef TABLASEGMENTOS_H
#define TABLASEGMENTOS_H

#include <stdint.h>
#include "consola.h"

#define CANT_SEG 8
#define CODE     0
#define DATA     1

/**
 * Tabla de descriptores de segmentos:
 * Consta de 8 entradas de 32 bits (variables de 32 bits):
 * - Los 16 bits mas significativos guardan la direccion fisica de comienzo (base).
 * - Los 16 bits menos significativos guardan el tamaño o limite en bytes.
 * Las entradas no utilizadas se inicializan con 0xFFFFFFFF (-1).
 */
typedef uint32_t tabla_segmentos[CANT_SEG];

void inicializarTablaSegmentos(tabla_segmentos tablaSegmentos);

/**
 * Carga la tabla de descriptores para MV1 (Version 1):
 * - Segmento 0: Codigo (base 0, tamano code_size)
 * - Segmento 1: Datos (base code_size, tamano TAMANIO_MEMORIA - code_size)
 * - Entradas 2..7 en 0xFFFFFFFF
 */
void cargarTablaSegmentosV1(tabla_segmentos tablaSegmentos, uint16_t code_size);

/**
 * Carga la tabla de descriptores para MV2 (Version 2):
 * - Calcula la memoria total del proceso y detecta memoria insuficiente antes de cargar.
 * - Ubica contiguamente los segmentos: Param -> Const -> Code -> Data -> Extra -> Stack.
 * - Solo crea entradas para segmentos con tamano mayor a cero (omite segmentos en cero).
 * - Entradas no utilizadas en 0xFFFFFFFF.
 */
void cargarTablaSegmentosV2(tabla_segmentos tablaSegmentos, const ConfiguracionMV *cfg);

uint16_t obtenerBaseSegmento(const tabla_segmentos tablaSegmentos, int segmento);
uint16_t obtenerTamanioSegmento(const tabla_segmentos tablaSegmentos, int segmento);
int esSegmentoValido(const tabla_segmentos tablaSegmentos, int segmento);

/**
 * Calcula el tamano del Param Segment a partir del archivo .vmx y parametros de -p.
 * Retorna 0 si no se indico el flag -p.
 */
uint16_t calcularTamanioParamSegment(const ConfiguracionMV *cfg);

/**
 * Muestra por consola las entradas de la tabla de descriptores de segmentos.
 */
void mostrarTablaSegmentos(const tabla_segmentos tablaSegmentos);

#endif // TABLASEGMENTOS_H
