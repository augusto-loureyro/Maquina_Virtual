
#ifndef consola_h
#define consola_h
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "loader.h"

#define MEMORIA_POR_DEFECTO_KIB 16

typedef struct {
    char *archVMX;           /** Ruta y nombre del archivo .vmx (NULL si no se especifico) */
    char *archVMI;           /** Ruta y nombre del archivo .vmi (NULL si no se especifico) */
    uint32_t mem_kib;        /** Tamaño de la memoria principal en KiB (por defecto 16 KiB) */
    int flagM;               //facu /** 1 se se especifico el flag -m, 0 en caso contrario */
    int flagD;               /** 1 si se especifico el flag -d, 0 en caso contrario */
    int flagP;               /** 1 si se especifico el flag -p, 0 en caso contrario */
    int cantParams;          /** Cantidad de parametros del proceso pasados tras -p */
    char **params;           /** Puntero al arreglo de cadenas de parametros */
    HeaderVMX header;        /** Metadatos del encabezado VMX (leidos por el loader) */
} ConfiguracionMV;

void inicializarConfiguracion(ConfiguracionMV *cfg);
void parsearArgumentos(int argc, char *argv[], ConfiguracionMV *cfg);

/**
 * Muestra por consola la configuracion parseada de la Maquina Virtual.
 */
void mostrarConfiguracion(const ConfiguracionMV *cfg);

/**
 * Muestra por consola todos los metadatos de configuracion y del encabezado VMX.
 */
void mostrarMetadatos(const ConfiguracionMV *cfg);
#endif 