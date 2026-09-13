#ifndef TRADUCTOR_H
#define TRADUCTOR_H

#include <stdint.h>
#include "tablaSegmentos.h"
#include "registros.h"
#include "errores.h"

typedef uint32_t dirLogica;
typedef uint32_t dirFisica;

// Traduce una dirección lógica a física.
//
// esAccesoAMemoria distingue dos casos, según pág. 7:
//  - 1: es un acceso real a memoria (resolver un operando). Se valida que
//       'cantidadBytes' entre dentro del segmento, y se cargan LAR (con la
//       dirección lógica) y MAR completo (cantidadBytes en la parte alta,
//       el resultado de esta traducción en la parte baja).
//  - 0: es la lectura de la instrucción (fetch). No se valida límite
//       (todavía no se sabe cuántos bytes ocupa la instrucción) y no se
//       tocan LAR ni MAR ("la lectura de la instrucción no debe modificar
//       ninguno de estos registros").
dirFisica traducir(dirLogica l, tabla_segmentos t, Registros r, int cantidadBytes, int esAccesoAMemoria);

/*anterior
// Traduce una dirección lógica a física. Si verificarLimite es 1, además
// valida que el acceso de 'cantidadBytes' entre dentro del segmento
// (excepción: la lectura de la instrucción, que llama con verificarLimite=0).
dirFisica traducir(dirLogica l, tabla_segmentos t, int cantidadBytes, int verificarLimite);
*/
#endif