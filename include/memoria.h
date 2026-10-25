#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>
#include "registros.h"
#include "consola.h"
#include "tablaSegmentos.h"

#define TAMANIO_MEMORIA 16384 // 16 KiB de RAM
#define TAMANIO_DATO    4     // Datos estándar de 32 bits (4 bytes)

typedef uint8_t *Memoria;

void reservarMemoria(Memoria *m, uint32_t tamanioM);
void liberarMemoria(Memoria *m);

/**
 * Carga el segmento de codigo para MV1 (Version 1).
 */
void cargarMemoriaV1(const char *nombreArchivo, Memoria m, uint16_t code_size);

/**
 * Carga byte a byte el Code Segment y el Const Segment para MV2 (Version 2).
 * Obtiene las bases fisicas directamente de la tabla de descriptores a traves de los registros CS y KS.
 */
void cargarMemoriaV2(const char *nombreArchivo, Memoria m, uint32_t tamanoRAM,
                      const tabla_segmentos t, const Registros r, const HeaderVMX *header);

/**
 * Carga el Param Segment para MV2 (Version 2).
 * Obtiene la base fisica del segmento desde la tabla de descriptores a traves del registro PS.
 */
void cargarParamSegment(const ConfiguracionMV *cfg, Memoria m, const tabla_segmentos t, const Registros r);

uint32_t leerDeMemoria(Memoria m, uint32_t f, int n, Registros r);
void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, int n, Registros r);

#endif // MEMORIA_H
