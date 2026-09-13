#include "memoria.h"
#include "errores.h"
#include <stdlib.h>

void reservarMemoria(Memoria *m,int tamanioM){
    *m = (uint8_t *)calloc(tamanioM, 1); //reservo 16384 bytes inicializados en 0...
}

void cargarMemoria(char *nombreArchivo, Memoria m){
    int i;
    FILE *archVMX;

    i = 0;
    if ((archVMX= fopen(nombreArchivo, "rb")) != NULL){
        fseek(archVMX, 8, SEEK_SET); // justo despues del header
        while (fread(&m[i], 1, 1, archVMX) == 1){
            i++;
        }

        fclose(archVMX);
    } else{
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }
}

