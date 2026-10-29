#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>
#include "registros.h"
#include "consola.h"
#include "tablaSegmentos.h"

#define TAMANIO_MEMORIA 16384 // 16 KiB de RAM
#define TAMANIO_DATO    4     // Datos estándar de 32 bits (4 bytes)

#define OFFSET_MEMORIA_VMI 168 //TAMANIO_HEADER_VMI + BYTES_REGISTROS_VMI + BYTES_TABLA_VMI  

typedef uint8_t *Memoria;

void reservarMemoria(Memoria *memoria, uint32_t tamanioM);
void liberarMemoria(Memoria *memoria);

/**
 * Carga el segmento de codigo para MV1 (Version 1).
 */
void cargarMemoriaV1(const char *nombreArchivo, Memoria memoria, uint16_t code_size);

/**
 * Carga byte a byte el Code Segment y el Const Segment para MV2 (Version 2).
 * Obtiene las bases fisicas directamente de la tabla de descriptores a traves de los registros CS y KS.
 */
void cargarMemoriaV2(const char *nombreArchivo, Memoria memoria, uint32_t tamanoRAM,
                      const tabla_segmentos tablaSegmentos, const Registros registros, const HeaderVMX *header);

/**
 * Carga el Param Segment para MV2 (Version 2).
 * Obtiene la base fisica del segmento desde la tabla de descriptores a traves del registro PS.
 */
void cargarParamSegment(const ConfiguracionMV *cfg, Memoria memoria, const tabla_segmentos tablaSegmentos, const Registros registros, uint32_t *argv,uint32_t *argc);

uint32_t leerDeMemoria(Memoria memoria, uint32_t dirFisica, int cantidadBytes, Registros registros);
void escribirEnMemoria(Memoria memoria, uint32_t dirFisica, uint32_t valor, int cantidadBytes, Registros registros);

void cargarMemoriaVMI(const char *nombreArchivo, Memoria memoria, uint32_t tamanioRAM);
#endif // MEMORIA_H
