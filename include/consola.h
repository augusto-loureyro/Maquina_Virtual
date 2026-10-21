#ifndef consola_h
#define consola_h
#include <stdio.h>
#include <string.h>

typedef struct{
    char archivoVMX[FILENAME_MAX];
    char archivoVMI[FILENAME_MAX];
    int flagD;
    int flagM;
    int flagP;
    int sizeM;     // tamaño de memoria leído con m=
    int p[100];    // parámetros enteros (asumo por ahora)
    int nP;        // cantidad de parámetros
} Config;

void parsearArgumentos(int argc, char *argv[], Config *cfg);

#endif