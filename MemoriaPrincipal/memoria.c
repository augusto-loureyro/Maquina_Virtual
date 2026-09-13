#include "../include/memoria.h"

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

uint32_t leerDeMemoria(Memoria m, uint32_t f, Registros r){
    uint32_t valor = ((uint32_t)m[f] << 24) | ((uint32_t)m[f+1] << 16) | ((uint32_t)m[f+2] << 8) | (uint32_t)m[f+3];
    r[REGMBR] = valor;
    return valor;
}
 
void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, Registros r){
    m[f]   = (valor >> 24) & 0xFF;
    m[f+1] = (valor >> 16) & 0xFF;
    m[f+2] = (valor >> 8) & 0xFF;
    m[f+3] = valor & 0xFF;
    r[REGMBR] = valor;
}