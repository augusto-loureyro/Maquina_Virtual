#include <stdlib.h>
#include <stdio.h>
#include "memoria.h"
#include "errores.h"

void reservarMemoria(Memoria *m, int tamanioM) {
    *m = (uint8_t *)calloc(tamanioM, sizeof(uint8_t));
    if (*m == NULL) {
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }
}

void liberarMemoria(Memoria *m) {
    if (m != NULL && *m != NULL) {
        free(*m);
        *m = NULL;
    }
}

void cargarMemoria(const char *nombreArchivo, Memoria m, uint16_t code_size) {
    FILE *archVMX = fopen(nombreArchivo, "rb");

    if (archVMX == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    // Saltar el encabezado de 8 bytes
    fseek(archVMX, 8, SEEK_SET);

    if (code_size > TAMANIO_MEMORIA) {
        fclose(archVMX);
        reportarError(ERROR_FALLO_DE_SEGMENTO); // El codigo no cabe en la memoria fisica
    }

    if (fread(m, 1, code_size, archVMX) != code_size) {
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO); // Archivo truncado
    }

    fclose(archVMX);
}

uint32_t leerDeMemoria(Memoria m, uint32_t f, int n, Registros r) {
    uint32_t valor = 0;
    for (int k = 0; k < n; k++) {
        valor = (valor << 8) | m[f + k]; // Big-endian
    }
    r[REGMBR] = valor;
    return valor;
}

void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, int n, Registros r) {
    r[REGMBR] = valor;
    for (int k = 0; k < n; k++) {
        m[f + k] = (valor >> (8 * (n - 1 - k))) & 0xFF; // Big-endian
    }
}
