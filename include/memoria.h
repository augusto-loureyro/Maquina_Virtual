#ifndef MEMORIA_VIRTUAL_H
#define MEMORIA_VIRTUAL_H

#define TAMANIO_MEMORIA 16384
#include<stdio.h>
#include<stdint.h>
#include "errores.h"
#include <stdlib.h>

typedef uint8_t *Memoria;

void reservarMemoria(Memoria *m,int tamanioM);
void cargarMemoria(char *nombreArchivo, Memoria m); //carga todo el codigo que sigue a la cabecera



#endif
