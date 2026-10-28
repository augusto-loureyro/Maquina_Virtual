#ifndef SYS_H
#define SYS_H

#include <stdint.h>
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "traductor.h"

// Códigos de llamadas al sistema
#define SYS_CLEAR_SCREEN 0x0
#define SYS_READ         0x1
#define SYS_WRITE        0x2
#define SYS_STRING_READ  0x3
#define SYS_STRING_WRITE 0x4
#define SYS_BREAKPOINT   0xF

// Modos de interpretación / salida (máscaras de bits)
#define MODO_DEC   0x01
#define MODO_CHAR  0x02
#define MODO_OCT   0x04
#define MODO_HEX   0x08
#define MODO_BIN   0x10


typedef struct {
    dirLogica dirL;     /** Dirección lógica inicial (EDX) */
    dirFisica dirF;     /** Dirección física inicial traducida */
    uint32_t modo;      /** Modo de lectura/escritura (EAX) */
    uint32_t cant;      /** Cantidad de valores (ECX bajo / ECX) */
    uint32_t tamBytes;  /** Tamaño de cada valor en bytes (ECX alto: 1..4 / 1) */
} ParamSys;


void llamadaSistema(uint32_t tipoLlamada, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros, ConfiguracionMV *cfg, int *breakpoint);

void sysWrite(ParamSys *paramSys, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros);
void sysRead(ParamSys *paramSys, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros);

int leerLinea(char *buf, int tamanioMax);
int parsearEntero(const char *cadena, int base, int conSigno, uint32_t *out);
int leerDecimal(const char *cadena, uint32_t *out);
int leerOctal(const char *cadena, uint32_t *out);
int leerHexa(const char *cadena, uint32_t *out);
int leerBinario(const char *cadena, uint32_t *out);
int leerChar(const char *cadena, uint32_t *out);
int leerValor(uint32_t modo, uint32_t *out);
int modoLecturaValido(uint32_t modo);

uint32_t mascaraBytes(uint32_t tamBytes);
int32_t extenderSigno(uint32_t valor, uint32_t bits);

void imprimirBinario(uint32_t valor);
void imprimirHexa(uint32_t valor);
void imprimirOctal(uint32_t valor);
void imprimirChar(uint32_t valor, uint32_t tamBytes);
void imprimirDecimal(uint32_t valor, uint32_t tamBytes);

#endif // SYS_H
