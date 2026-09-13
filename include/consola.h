/*
    Que consola 
        parsee los argumentos (solo verifica que no haya argumentos invalidos)
        verifique la extension .vmx del archivo
*/

#ifndef consola_h
#define consola_h
#include <string.h>
#include "errores.h"

void parsearArgumentos(int argc, char *argv[], char **nombreArchivo, int *flagD);

#endif