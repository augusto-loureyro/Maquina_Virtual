#include "sys.h"
#include "traductor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/// Decodifica EAX/ECX/EDX y valida. Devuelve 1 si hay algo para hacer, 0 si no.
int prepararSys(ParamSys *p, tabla_segmentos t, Registros r){
    uint32_t ecx = r[REGECX];

    p->dirL     = r[REGEDX];
    p->modo     = r[REGEAX];
    p->tamBytes = ecx >> 16;        // 2 bytes altos: tamaño de cada valor
    p->cant     = ecx & 0xFFFF;     // 2 bytes bajos: cantidad de valores

    if (p->tamBytes < 1 || p->tamBytes > 4)
        reportarError(ERROR_TAMANIO_SYS);     

    if (p->cant == 0)
        return 0;                   // nada que hacer

    /// Valida que TODO el rango entre en el segmento (si no, traducir aborta)
    p->dirF = traducir(p->dirL, t, r, p->tamBytes * p->cant, 1); //!!!# 
    return 1;
}

void sysWrite(ParamSys *p, Memoria m, tabla_segmentos t, Registros r){
    for (uint32_t i = 0; i < p->cant; i++){
        dirLogica l = p->dirL + i * p->tamBytes;

        dirFisica f    = traducir(l, t, r, p->tamBytes, 1);   /// LAR y MAR
        uint32_t valor = leerDeMemoria(m, f, p->tamBytes, r); /// MBR

        printf("[%04X]:", (unsigned)f);
        if (p->modo & MODO_BIN)  imprimirBinario(valor, p->tamBytes);
        if (p->modo & MODO_HEX)  imprimirHexa(valor);
        if (p->modo & MODO_OCT)  imprimirOctal(valor);
        if (p->modo & MODO_CHAR) imprimirChar(valor);
        if (p->modo & MODO_DEC)  imprimirDecimal(valor, p->tamBytes);
        printf("\n");
    }
}
void sysRead(ParamSys *p, Memoria m, tabla_segmentos t, Registros r){
    uint32_t mascara = mascaraBytes(p->tamBytes);

    if (!modoLecturaValido(p->modo))
        reportarError(ERROR_MODO_SYS);

    for (uint32_t i = 0; i < p->cant; i++){
        dirLogica l = p->dirL + i * p->tamBytes;
        uint32_t valor = 0;
        int ok;

        dirFisica f = traducir(l, t, r, p->tamBytes, 1);      /// una sola vez

        do {
            printf("[%04X]: ", (unsigned)f);
            fflush(stdout);
            ok = leerValor(p->modo, &valor);
        } while (!ok);

        escribirEnMemoria(m, f, valor & mascara, p->tamBytes, r);
    }
}

void llamadaSistema(uint32_t tipoLlamada, Memoria m, tabla_segmentos t, Registros r){
    ParamSys p;

    if (tipoLlamada != SYS_READ && tipoLlamada != SYS_WRITE)
        reportarError(ERROR_SYS_INVALIDA);

    if (!prepararSys(&p, t, r))
        return;

    if (tipoLlamada == SYS_WRITE) 
        sysWrite(&p, m, t, r);
    else                          
        sysRead(&p, m, t, r);
}

//---
int leerLinea(char *buf, int n){
    if (!fgets(buf, n, stdin))
        return 0;                                   /// EOF
    if (!strchr(buf, '\n')){                        /// línea más larga que el buffer:
        int c;                                      /// descarto el resto para no
        while ((c = getchar()) != '\n' && c != EOF);/// contaminar la próxima lectura
    }
    return 1;
}
int parsearEntero(const char *s, int base, int conSigno, uint32_t *out){
    char *fin;

    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return 0;
    if (!conSigno && *s == '-') return 0;           /// octal/hexa/binario no admiten signo

    errno = 0;
    if (conSigno){
        long long v = strtoll(s, &fin, base);
        if (errno || fin == s || v < INT32_MIN || v > (long long)UINT32_MAX) return 0;
        *out = (uint32_t)v;
    } else {
        unsigned long long v = strtoull(s, &fin, base);
        if (errno || fin == s || v > UINT32_MAX) return 0;
        *out = (uint32_t)v;
    }

    while (isspace((unsigned char)*fin)) fin++;
    return *fin == '\0';                            /// no puede sobrar basura
}
int leerDecimal(const char *s, uint32_t *out){ return parsearEntero(s, 10, 1, out); }
int leerOctal  (const char *s, uint32_t *out){ return parsearEntero(s,  8, 0, out); }
int leerHexa   (const char *s, uint32_t *out){ return parsearEntero(s, 16, 0, out); }
int leerBinario(const char *s, uint32_t *out){ return parsearEntero(s,  2, 0, out); }

int leerChar(const char *s, uint32_t *out){
    if (s[0] == '\0' || s[0] == '\n') return 0;
    *out = (unsigned char)s[0];
    return 1;
}
int leerValor(uint32_t modo, uint32_t *out){
    char linea[64];

    if (!leerLinea(linea, sizeof linea))
        reportarError(ERROR_ENTRADA_SYS);           /// EOF: no hay forma de seguir

    switch (modo){
        case MODO_DEC:  return leerDecimal(linea, out);
        case MODO_CHAR: return leerChar(linea, out);
        case MODO_OCT:  return leerOctal(linea, out);
        case MODO_HEX:  return leerHexa(linea, out);
        case MODO_BIN:  return leerBinario(linea, out);
    }
    return 0;
}
int modoLecturaValido(uint32_t modo){
    return modo == MODO_DEC || modo == MODO_CHAR || modo == MODO_OCT || modo == MODO_HEX || modo == MODO_BIN;
}
uint32_t mascaraBytes(uint32_t tamBytes){
    return (tamBytes >= 4) ? 0xFFFFFFFFu : ((1u << (tamBytes * 8)) - 1u);
}

int32_t extenderSigno(uint32_t v, uint32_t bits){
    if (bits >= 32) return (int32_t)v;
    uint32_t signo = 1u << (bits - 1);
    return (int32_t)((v ^ signo) - signo);
}

void imprimirBinario(uint32_t v, uint32_t tamBytes){
    printf(" 0b");
    for (int j = (int)(tamBytes * 8) - 1; j >= 0; j--)
        printf("%u", (v >> j) & 1u);
}
void imprimirHexa(uint32_t v)  { printf(" 0x%X", v); }
void imprimirOctal(uint32_t v) { printf(" 0o%o", v); }
void imprimirChar(uint32_t v)  { printf(" %c", (v >= 32 && v <= 126) ? (char)v : '.'); }
void imprimirDecimal(uint32_t v, uint32_t tamBytes){
    printf(" %d", extenderSigno(v, tamBytes * 8));
}