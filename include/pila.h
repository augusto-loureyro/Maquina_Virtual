#ifndef PILA_H
#define PILA_H

#include <stdint.h>
#include "memoria.h"
#include "registros.h"
#include "tablaSegmentos.h"

/**
 * Operaciones sobre la pila (Stack Segment) de la Maquina Virtual MV2:
 * La pila crece hacia las posiciones inferiores de memoria (hacia abajo).
 * Todas las operaciones de pila almacenan o extraen celdas de 4 bytes en formato big-endian.
 */


 
/**
 * 1. Decrementa SP en 4.
 * 2. Si el valor de SP queda por debajo de SS (o si no hay Stack Segment), reporta ERROR_STACK_OVERFLOW.
 * 3. Almacena en la memoria apuntada por SP el valor de 4 bytes en orden big-endian.
 */
void push(uint32_t valor, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros);


/**
 * 1. Verifica si la pila esta vacia o si no hay al menos 4 bytes disponibles para extraer.
 *    En tal caso, reporta ERROR_STACK_UNDERFLOW.
 * 2. Lee los 4 bytes apuntados por SP en orden big-endian sin alterar el contenido de la memoria.
 * 3. Incrementa SP en 4.
 * 4. Retorna el valor extraido de 4 bytes.
 */
uint32_t pop(tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros);

#endif // PILA_H
