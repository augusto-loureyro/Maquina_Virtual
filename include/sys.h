#ifndef SYS_H
#define SYS_H

#include <stdint.h>
#include "tablaSegmentos.h"
#include "memoria.h"
#include "traductor.h"

#define SYS_READ   0x1
#define SYS_WRITE  0x2

#define MODO_DEC   0x01
#define MODO_CHAR  0x02
#define MODO_OCT   0x04
#define MODO_HEX   0x08
#define MODO_BIN   0x10

typedef struct {
    dirLogica dirL;
    dirFisica dirF;
    uint32_t modo;
    uint32_t cant;
    uint32_t tamBytes;
} ParamSys;

void llamadaSistema(uint32_t tipoLlamada, Memoria m, tabla_segmentos t, Registros r);

void sysWrite(ParamSys *p, Memoria m, tabla_segmentos t, Registros r);
void sysRead(ParamSys *p, Memoria m, tabla_segmentos t, Registros r);


int leerLinea(char *buf, int n);
int parsearEntero(const char *s, int base, int conSigno, uint32_t *out);
int leerDecimal(const char *s, uint32_t *out);
int leerOctal  (const char *s, uint32_t *out);
int leerHexa   (const char *s, uint32_t *out);
int leerBinario(const char *s, uint32_t *out);
int leerChar(const char *s, uint32_t *out);
int leerValor(uint32_t modo, uint32_t *out);
int modoLecturaValido(uint32_t modo);
uint32_t mascaraBytes(uint32_t tamBytes);
int32_t extenderSigno(uint32_t v, uint32_t bits);
void imprimirBinario(uint32_t v, uint32_t tamBytes);
void imprimirHexa(uint32_t v);
void imprimirOctal(uint32_t v);
void imprimirChar(uint32_t v);
void imprimirDecimal(uint32_t v, uint32_t tamBytes);

#endif