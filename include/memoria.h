#ifndef MEMORIA_H
#define MEMORIA_H

#include<stdint.h>
#include "registros.h"

#define TAMANIO_MEMORIA 16384
#define TAMANIO_DATO 4 // los registros (y por lo tanto los accesos a memoria) son de 32 bits

typedef uint8_t *Memoria;

void reservarMemoria(Memoria *m,int tamanioM);
void cargarMemoria(char *nombreArchivo, Memoria m,uint16_t code_size);//carga todo el codigo que sigue a la cabecera
uint32_t leerDeMemoria(Memoria m, uint32_t f, int n, Registros r);
void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, int n, Registros r);

#endif