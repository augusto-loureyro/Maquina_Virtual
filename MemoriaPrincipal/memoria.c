#include <stdlib.h>
#include<stdio.h>

#include "memoria.h"
#include "errores.h"


void reservarMemoria(Memoria *m,int tamanioM){
    *m = (uint8_t *)calloc(tamanioM, 1); //reservo 16384 bytes inicializados en 0...
}

/*
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
*/
void cargarMemoria(char *nombreArchivo, Memoria m, uint16_t code_size){
    FILE *archVMX;

    if ((archVMX = fopen(nombreArchivo, "rb")) == NULL){
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    fseek(archVMX, 8, SEEK_SET);

    if (code_size > TAMANIO_MEMORIA) {
        fclose(archVMX);
        reportarError(ERROR_FALLO_DE_SEGMENTO); // el código no entra en memoria
    }

    if (fread(m, 1, code_size, archVMX) != code_size) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO); // archivo truncado: menos bytes de los que declara el header
    }

    fclose(archVMX);
}

/// MBR -> al leer de, y al escribir en, memoria, se actualiza con el valor a escribir/leer
uint32_t leerDeMemoria(Memoria m, uint32_t f, int n, Registros r){
    uint32_t valor = 0;
    for (int k = 0; k < n; k++)
        valor = (valor << 8) | m[f + k];    /// big-endian, n bytes
    r[REGMBR] = valor;
    return valor;
}
void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, int n, Registros r){
    r[REGMBR] = valor;                       
    for (int k = 0; k < n; k++)
        m[f + k] = (valor >> (8 * (n - 1 - k))) & 0xFF;
}