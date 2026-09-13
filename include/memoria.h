#ifndef MEMORIA_H
#define MEMORIA_H


#include<stdio.h>
#include<stdint.h>
#include "errores.h"
#include <stdlib.h>
#include "registros.h"

#define TAMANIO_MEMORIA 16384
#define TAMANIO_DATO 4 // los registros (y por lo tanto los accesos a memoria) son de 32 bits

typedef uint8_t *Memoria;

void reservarMemoria(Memoria *m,int tamanioM);
void cargarMemoria(char *nombreArchivo, Memoria m); //carga todo el codigo que sigue a la cabecera

// Leen/escriben un dato de TAMANIO_DATO bytes (big-endian) en una dirección
// FÍSICA ya traducida, y cargan MBR con el valor operado, según pág. 7:
// "En el registro MBR debe quedar el valor con el cual se está operando".
uint32_t leerDeMemoria(Memoria m, uint32_t direccionFisica, Registros r);
void escribirEnMemoria(Memoria m, uint32_t direccionFisica, uint32_t valor, Registros r);

#endif