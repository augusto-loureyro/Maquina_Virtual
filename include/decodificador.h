#ifndef DECODIFICADOR_H
#define DECODIFICADOR_H

#include <stdint.h>
#include "errores.h"

// Categorias de instruccion por cantidad de operandos
#define SIN_OPERANDOS  0
#define UN_OPERANDO    1
#define DOS_OPERANDOS  2

// Tipos de operandos (su codigo binario coincide con su tamaño en bytes)
#define TIPO_NINGUNO   0  // 0 bytes
#define TIPO_REGISTRO  1  // 1 byte
#define TIPO_INMEDIATO 2  // 2 bytes
#define TIPO_MEMORIA   3  // 3 bytes

/**
 * Estructura que almacena la información de un operando decodificado.
 */
typedef struct {
    int tipo;               /** TIPO_NINGUNO, TIPO_REGISTRO, TIPO_INMEDIATO o TIPO_MEMORIA */
    int codigoRegistro;     /** Codigo de registro (0..31), valido en TIPO_REGISTRO y TIPO_MEMORIA */
    int16_t desplazamiento; /** Offset con signo, valido en TIPO_MEMORIA */
    int16_t inmediato;      /** Valor entero de 16 bits con signo, valido en TIPO_INMEDIATO */
} Operando;


void decodificarPrimerByte(uint8_t primerByte, int *opcode, int *categoria);
void extraerTiposOperando(uint8_t primerByte, int categoria, int *tipoA, int *tipoB);
int tamanioOperando(int tipo);
Operando leerOperando(int tipo, uint8_t *bytes);

#endif // DECODIFICADOR_H
