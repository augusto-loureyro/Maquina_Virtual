#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "sys.h"
#include "traductor.h"
#include "errores.h"

int prepararSys(ParamSys *paramSys, uint32_t tipoLlamada, tabla_segmentos tablaSegmentos, Registros registros) {
    uint32_t ecx = registros[REGECX];

    paramSys->dirL = registros[REGEDX];
    paramSys->cant = 0;

    switch (tipoLlamada) {
        case SYS_READ:
        case SYS_WRITE:
            paramSys->modo     = registros[REGEAX];
            paramSys->cant     = ecx & 0xFFFF;     // 2 bytes menos significativos: cantidad de elementos
            paramSys->tamBytes = ecx >> 16;        // 2 bytes mas significativos: tamaño de cada elemento (1..4)
            break;
        case SYS_STRING_READ:
            paramSys->cant = ecx;
        case SYS_STRING_WRITE:
            paramSys->tamBytes = 1;
            break;
    }

    if (paramSys->cant == 0 && tipoLlamada != SYS_STRING_WRITE)
        return 0; // Nada que transferir

    if (paramSys->tamBytes < 1 || paramSys->tamBytes > 4)
        reportarError(ERROR_TAMANIO_SYS);

    if (tipoLlamada == SYS_STRING_WRITE || (tipoLlamada == SYS_STRING_READ && ecx == -1))
        paramSys->dirF = traducir(paramSys->dirL, tablaSegmentos, registros, 0, 0);
    else
        paramSys->dirF = traducir(paramSys->dirL, tablaSegmentos, registros, paramSys->tamBytes * paramSys->cant, 1);

    return 1;
}

void sysWrite(ParamSys *paramSys, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros) {
    for (uint32_t i = 0; i < paramSys->cant; i++) {
        dirLogica dirLog = paramSys->dirL + i * paramSys->tamBytes;
        dirFisica dirFis = traducir(dirLog, tablaSegmentos, registros, paramSys->tamBytes, 1);
        uint32_t valor   = leerDeMemoria(memoria, dirFis, paramSys->tamBytes, registros);

        printf("[%04X]:", (unsigned)dirFis);
        if (paramSys->modo & MODO_BIN)
            imprimirBinario(valor);
        if (paramSys->modo & MODO_HEX)
            imprimirHexa(valor);
        if (paramSys->modo & MODO_OCT)
            imprimirOctal(valor);
        if (paramSys->modo & MODO_CHAR)
            imprimirChar(valor, paramSys->tamBytes);
        if (paramSys->modo & MODO_DEC)
            imprimirDecimal(valor, paramSys->tamBytes);
        printf("\n");
    }
}

void sysStringWrite(ParamSys *paramSys, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros) {
    dirLogica dirLog = paramSys->dirL;
    dirFisica dirFis = traducir(dirLog, tablaSegmentos, registros, 1, 1);
    uint8_t valor = leerDeMemoria(memoria, dirFis, 1, registros);

    printf("[%04X]: ", (unsigned)dirFis);

    while (valor != '\0') {
        putchar(valor);
        dirLog++;
        dirFis = traducir(dirLog, tablaSegmentos, registros, 1, 1);
        valor = leerDeMemoria(memoria, dirFis, 1, registros);
    }
}

void sysRead(ParamSys *paramSys, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros) {
    uint32_t mascara = mascaraBytes(paramSys->tamBytes);

    if (!modoLecturaValido(paramSys->modo))
        reportarError(ERROR_MODO_SYS);

    for (uint32_t i = 0; i < paramSys->cant; i++) {
        dirLogica dirLog = paramSys->dirL + i * paramSys->tamBytes;
        uint32_t valor = 0;
        int ok;

        dirFisica dirFis = traducir(dirLog, tablaSegmentos, registros, paramSys->tamBytes, 1);

        do {
            printf("[%04X]: ", (unsigned)dirFis);
            fflush(stdout);
            ok = leerValor(paramSys->modo, &valor);
        } while (!ok);

        escribirEnMemoria(memoria, dirFis, valor & mascara, paramSys->tamBytes, registros);
    }
}

void sysStringRead(ParamSys *paramSys, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros) {
    char linea[64];
    uint32_t limite = paramSys->cant == -1 ? 64 : paramSys->cant;
    dirLogica dirLog = paramSys->dirL;
    dirFisica dirFis = traducir(dirLog, tablaSegmentos, registros, 1, 1);

    printf("[%04X]: ", (unsigned)dirFis);
    fflush(stdout);

    if (!leerLinea(linea, sizeof(linea)))
        reportarError(ERROR_ENTRADA_SYS);

    uint32_t i = 0;

    while (i < limite && linea[i] != '\0') {
        escribirEnMemoria(memoria, dirFis, linea[i], 1, registros);
        i++;
        dirLog++;
        dirFis = traducir(dirLog, tablaSegmentos, registros, 1, 1);
    }

    escribirEnMemoria(memoria, dirFis, '\0', 1, registros);
}

char sysBreakpoint(char *nombreArch, uint16_t mem_kib, Registros registros, tabla_segmentos tablaSegmentos, Memoria memoria) {
    FILE *arch = fopen(nombreArch, "wb");

    if (arch == NULL)
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);

    // Generar imagen
    // Header
    fwrite("VMI26", 1, 5, arch);
    uint8_t version = 1;
    fwrite(&version, 1, 1, arch);
    fwrite(&mem_kib, 2, 1, arch);

    // Registros
    fwrite(registros, sizeof(registros[0]), CANT_REGISTROS, arch);
    // Tabla segmentos
    fwrite(tablaSegmentos, sizeof(tablaSegmentos[0]), CANT_SEG, arch);
    // Memoria principal
    fwrite(memoria, sizeof(memoria[0]), mem_kib * 1024, arch);

    fclose(arch);

    // Pausa y modo debug
    int input, c;
    do
    {
        input = getchar();
    } while (input != 'g' && input != 'q' && input != '\n' && input != EOF);

    if (input != '\n' && input != EOF)
        while ((c = getchar()) != '\n' && c != EOF);

    return input == EOF ? '.' : (char)input;
}

void llamadaSistema(uint32_t tipoLlamada, Memoria memoria, tabla_segmentos tablaSegmentos, Registros registros, ConfiguracionMV *cfg, int *breakpoint) {
    ParamSys paramSys;

    switch(tipoLlamada) {
        case SYS_READ:
            if (prepararSys(&paramSys, tipoLlamada, tablaSegmentos, registros))
                sysRead(&paramSys, memoria, tablaSegmentos, registros);
            break;
        case SYS_WRITE:
            if (prepararSys(&paramSys, tipoLlamada, tablaSegmentos, registros))
                sysWrite(&paramSys, memoria, tablaSegmentos, registros);
            break;
        case SYS_STRING_READ:
            if (prepararSys(&paramSys, tipoLlamada, tablaSegmentos, registros))
                sysStringRead(&paramSys, memoria, tablaSegmentos, registros);
            break;
        case SYS_STRING_WRITE:
            if (prepararSys(&paramSys, tipoLlamada, tablaSegmentos, registros))
                sysStringWrite(&paramSys, memoria, tablaSegmentos, registros);
            break;
        case SYS_CLEAR_SCREEN:
            printf("\033[2J\033[H"); // \033[2J borra toda la pantalla, \033[H mueve el cursor a la posicion 0,0
            fflush(stdout);
            break;
        case SYS_BREAKPOINT:
            if (cfg->archVMI == NULL)
                return; 
            char input = sysBreakpoint(cfg->archVMI, (uint16_t)cfg->mem_kib, registros, tablaSegmentos, memoria);
            switch(input) {
                case 'q':
                    exit(0);
                    break;
                case '\n':
                    *breakpoint = 1;
                    break;
                default:
                    *breakpoint = 0;
                    break;
            }
            break;
    }
}

/// Helpers de Lectura y Parseo

int leerLinea(char *buf, int tamanioMax) {
    if (!fgets(buf, tamanioMax, stdin)) {
        return 0; // EOF
    }
    if (!strchr(buf, '\n')) {
        int caracter;
        while ((caracter = getchar()) != '\n' && caracter != EOF);
    }
    return 1;
}

int parsearEntero(const char *cadena, int base, int conSigno, uint32_t *out) {
    char *fin;

    while (isspace((unsigned char)*cadena)) cadena++;
    if (*cadena == '\0')
        return 0;
    if (!conSigno && *cadena == '-')
        return 0;

    errno = 0;
    if (conSigno) {
        long long valorLeido = strtoll(cadena, &fin, base);
        if (errno || fin == cadena || valorLeido < INT32_MIN || valorLeido > (long long)UINT32_MAX) return 0;
        *out = (uint32_t)valorLeido;
    } else {
        unsigned long long valorLeido = strtoull(cadena, &fin, base);
        if (errno || fin == cadena || valorLeido > UINT32_MAX) return 0;
        *out = (uint32_t)valorLeido;
    }

    while (isspace((unsigned char)*fin))
        fin++;
    return *fin == '\0';
}

int leerDecimal(const char *cadena, uint32_t *out) {
    return parsearEntero(cadena, 10, 1, out);
}

int leerOctal  (const char *cadena, uint32_t *out) {
    return parsearEntero(cadena,  8, 0, out);
}

int leerHexa   (const char *cadena, uint32_t *out) {
    return parsearEntero(cadena, 16, 0, out);
}

int leerBinario(const char *cadena, uint32_t *out) {
    return parsearEntero(cadena,  2, 0, out);
}

int leerChar(const char *cadena, uint32_t *out) {
    if (cadena[0] == '\0' || cadena[0] == '\n') return 0;
    *out = (unsigned char)cadena[0];
    return 1;
}

int leerValor(uint32_t modo, uint32_t *out) {
    char linea[64];

    if (!leerLinea(linea, sizeof(linea))) {
        reportarError(ERROR_ENTRADA_SYS);
    }

    switch (modo) {
        case MODO_DEC:  return leerDecimal(linea, out);
        case MODO_CHAR: return leerChar(linea, out);
        case MODO_OCT:  return leerOctal(linea, out);
        case MODO_HEX:  return leerHexa(linea, out);
        case MODO_BIN:  return leerBinario(linea, out);
        default:        return 0;
    }
}

int modoLecturaValido(uint32_t modo) {
    return (modo == MODO_DEC || modo == MODO_CHAR || modo == MODO_OCT || modo == MODO_HEX || modo == MODO_BIN);
}

uint32_t mascaraBytes(uint32_t tamBytes) {
    return (tamBytes >= 4) ? 0xFFFFFFFFu : ((1u << (tamBytes * 8)) - 1u);
}

int32_t extenderSigno(uint32_t valor, uint32_t bits) {
    if (bits >= 32) return (int32_t)valor;
    uint32_t signo = 1u << (bits - 1);
    return (int32_t)((valor ^ signo) - signo);
}

/// Helpers de Formateo e Impresion

void imprimirBinario(uint32_t valor) {
    printf(" 0b");
    if (valor == 0) {
        printf("0");
        return;
    }
    int bitMasSignificativo = 31;
    while (bitMasSignificativo >= 0 && !((valor >> bitMasSignificativo) & 1u)) {
        bitMasSignificativo--;
    }
    for (int j = bitMasSignificativo; j >= 0; j--) {
        printf("%u", (valor >> j) & 1u);
    }
}

void imprimirHexa(uint32_t valor) {
    printf(" 0x%X", valor);
}

void imprimirOctal(uint32_t valor) {
    printf(" 0o%o", valor);
}

void imprimirChar(uint32_t valor, uint32_t tamBytes) {
    printf(" ");
    for (int j = (int)tamBytes - 1; j >= 0; j--) {
        unsigned char caracter = (valor >> (j * 8)) & 0xFF;
        putchar((caracter >= 32 && caracter <= 126) ? (char)caracter : '.');
    }
}

void imprimirDecimal(uint32_t valor, uint32_t tamBytes) {
    printf(" %d", extenderSigno(valor, tamBytes * 8));
}
