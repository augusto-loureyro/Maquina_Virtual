#ifndef CONSOLA_H
#define CONSOLA_H

#include <stdint.h>
#include "loader.h"

#define MEMORIA_POR_DEFECTO_KIB 16

/**
 * Estructura de configuracion de la Maquina Virtual.
 * Centraliza los argumentos de linea de comandos y los metadatos del archivo VMX.
 */
typedef struct {
    char *archVMX;           /** Ruta y nombre del archivo .vmx (NULL si no se especifico) */
    char *archVMI;           /** Ruta y nombre del archivo .vmi (NULL si no se especifico) */
    uint32_t mem_kib;        /** Tamaño de la memoria principal en KiB (por defecto 16 KiB) */
    int flagD;               /** 1 si se especifico el flag -d, 0 en caso contrario */
    int flagP;               /** 1 si se especifico el flag -p, 0 en caso contrario */
    int cantParams;          /** Cantidad de parametros del proceso pasados tras -p */
    char **params;           /** Puntero al arreglo de cadenas de parametros */
    HeaderVMX header;        /** Metadatos del encabezado VMX (leidos por el loader) */
    HeaderVMI headerVMI;
} ConfiguracionMV;

/**
 * Inicializa la estructura de configuracion con sus valores predeterminados.
 */
void inicializarConfiguracion(ConfiguracionMV *cfg);

/**
 * Valida los argumentos de linea de comandos.
 *
 * Sintaxis:
 *   vmx [filename.vmx] [filename.vmi] [m=M] [-d] [-p param1 param2 ... paramN]
 *
 * - Es obligatorio al menos uno de los dos archivos (.vmx o .vmi).
 * - Si no se especifica .vmx, se ignoran los parametros de -p.
 * - El flag -p siempre debe ubicarse al final del comando.
 */
void parsearArgumentos(int argc, char *argv[], ConfiguracionMV *cfg);

/**
 * Muestra por consola la configuracion parseada de la Maquina Virtual.
 */
void mostrarConfiguracion(const ConfiguracionMV *cfg);

/**
 * Muestra por consola todos los metadatos de configuracion y del encabezado VMX.
 */
void mostrarMetadatos(const ConfiguracionMV *cfg);

#endif // CONSOLA_H

