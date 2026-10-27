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

// Sectores de registro (bits 6-5 del byte de operando de registro)
#define SECTOR_COMPLETO   0 // 00: 4 bytes (EAX..EFX o cualquier registro)
#define SECTOR_BYTE_BAJO  1 // 01: 4to byte del registro (AL..FL)
#define SECTOR_BYTE_ALTO  2 // 10: 3er byte del registro (AH..FH)
#define SECTOR_WORD_BAJO  3 // 11: 2 bytes bajos (AX..FX)

// Modificadores de operando de memoria (bits 7-5 del 3er byte de memoria)
#define MODIF_MEM_DEFAULT 0 // 000: 4 bytes (sin prefijo [])
#define MODIF_MEM_BYTE    1 // 001: 1 byte  (prefijo b[])
#define MODIF_MEM_WORD    2 // 010: 2 bytes (prefijo w[])
#define MODIF_MEM_LONG    4 // 100: 4 bytes (prefijo l[])

/**
 * Estructura que almacena la información de un operando decodificado.
 */
typedef struct {
    int tipo;               /** TIPO_NINGUNO, TIPO_REGISTRO, TIPO_INMEDIATO o TIPO_MEMORIA */
    int codigoRegistro;     /** Codigo de registro (0..31), valido en TIPO_REGISTRO y TIPO_MEMORIA */
    int sector;             /** Sector del registro: SECTOR_COMPLETO, SECTOR_BYTE_BAJO, SECTOR_BYTE_ALTO, SECTOR_WORD_BAJO */
    int tamanoMemoria;      /** Tamano del dato en memoria en bytes: 1 (b), 2 (w), 4 (l o []) */
    int modifMemoria;       /** Modificador binario de memoria (0, 1, 2, 4) */
    int16_t desplazamiento; /** Offset con signo, valido en TIPO_MEMORIA */
    int16_t inmediato;      /** Valor entero de 16 bits con signo, valido en TIPO_INMEDIATO */
} Operando;

void decodificarPrimerByte(uint8_t primerByte, int *opcode, int *categoria);
void extraerTiposOperando(uint8_t primerByte, int categoria, int *tipoA, int *tipoB);
int tamanioOperando(int tipo);
Operando leerOperando(int tipo, uint8_t *bytes);
int obtenerTamanoOperando(Operando operando);
uint32_t truncarYExtenderSigno(uint32_t val, int tamanoBytes);

#endif // DECODIFICADOR_H
