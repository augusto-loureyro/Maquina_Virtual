#ifndef DECODIFICADOR_H
#define DECODIFICADOR_H

#include <stdint.h>
#include "errores.h"

#define DOS_OPERANDOS 2
#define UN_OPERANDO 1
#define SIN_OPERANDOS 0

#define TIPO_NINGUNO 0
#define TIPO_REGISTRO 1
#define TIPO_INMEDIATO 2
#define TIPO_MEMORIA 3

typedef struct {
    int tipo;
    int codigoRegistro;   // válido si tipo == TIPO_REGISTRO o TIPO_MEMORIA
    int16_t desplazamiento; // válido si tipo == TIPO_MEMORIA
    int16_t inmediato;      // válido si tipo == TIPO_INMEDIATO
} Operando;

void decodificarPrimerByte(uint8_t primerByte, int *opcode, int *categoria);
void extraerTiposOperando(uint8_t primerByte, int categoria, int *tipoA, int *tipoB);
int tamanioOperando(int tipo);
Operando leerOperando(int tipo, uint8_t *bytes);

#endif