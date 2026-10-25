/*
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/consola.h"
#include "include/errores.h"

void parsearArgumentos(int argc, char *argv[], Config *cfg) {
    // Inicializar estructura
    memset(cfg, 0, sizeof(Config));

    if (argc < 2) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    for (int i = 1; i < argc; i++) {
        size_t len = strlen(argv[i]);

        // Archivos
        if (len >= 4 && strcmp(argv[i] + len - 4, ".vmx") == 0) {
            strncpy(cfg->archivoVMX, argv[i], FILENAME_MAX - 1);
        } else if (len >= 4 && strcmp(argv[i] + len - 4, ".vmi") == 0) {
            strncpy(cfg->archivoVMI, argv[i], FILENAME_MAX - 1);
        } else if (strcmp(argv[i], "-d") == 0) { // Flags
            cfg->flagD = 1;
        } else if (strncmp(argv[i], "m=", 2) == 0 || strncmp(argv[i], "M=", 2) == 0) {
                cfg->flagM = 1;
                cfg->sizeM = atoi(argv[i] + 2);
                if (cfg->sizeM <= 0 || cfg->sizeM > 65535)
                    reportarError(ERROR_ARGUMENTOS_INVALIDOS);
        } else if (strcmp(argv[i], "-p") == 0) {
            cfg->flagP = 1;
            for (int j = i + 1; j < argc; j++) {
                cfg->p[cfg->nP++]=argv[j];
            }
            break; // después de -p, todo lo que sigue son parámetros
        } else {
            reportarError(ERROR_ARGUMENTOS_INVALIDOS);
            // Si pusiera multiples VMX y/o VMI, lo aceptaría y quedaría el último, supongo que no es importante. 
        }
    }
    // Validación: al menos un archivo válido
    if (cfg->archivoVMX[0] == '\0' && cfg->archivoVMI[0] == '\0') {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }
}
*/

// su consola.c parece hacer lo mismo y alguna verificaciñon más:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "consola.h"
#include "errores.h"

void inicializarConfiguracion(ConfiguracionMV *cfg) {
    cfg->archVMX = NULL;
    cfg->archVMI = NULL;
    cfg->mem_kib = MEMORIA_POR_DEFECTO_KIB;
    cfg->flagM = 0;
    cfg->flagD = 0;
    cfg->flagP = 0;
    cfg->cantParams = 0;
    cfg->params = NULL;
    memset(&cfg->header, 0, sizeof(HeaderVMX));
}

static int tieneExtension(const char *nombre, const char *ext) {
    size_t len = strlen(nombre);
    size_t extLen = strlen(ext);
    if (len <= extLen) {
        return 0;
    } else {
        const char *p = nombre + len - extLen;
        for (size_t i = 0; i < extLen; i++) {
            if (tolower((unsigned char)p[i]) != tolower((unsigned char)ext[i])) {
                return 0;
            }
        }
        return 1;
    }
}

static int parsearParametroMemoria(const char *arg, uint32_t *mem_kib) {
    if ((arg[0] != 'm' && arg[0] != 'M') || arg[1] != '=') {
        return 0;
    }

    const char *valStr = arg + 2;
    if (*valStr == '\0') {
        return 0;
    }

    for (int i = 0; valStr[i] != '\0'; i++) {
        if (!isdigit((unsigned char)valStr[i])) {
            return 0;
        }
    }

    char *endptr = NULL;
    unsigned long val = strtoul(valStr, &endptr, 10);
    if (endptr == NULL || *endptr != '\0' || val == 0 || val > 65535) {
        return 0;
    }

    *mem_kib = (uint32_t)val;
    return 1;
}

void parsearArgumentos(int argc, char *argv[], ConfiguracionMV *cfg) {
    int memEspecificada = 0;

    if (argc < 2) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    } else {
        for (int i = 1; i < argc; i++) {
            /// Flag de parametros: siempre al final del comando
            if (strcmp(argv[i], "-p") == 0) {
                cfg->flagP = 1;
                cfg->cantParams = argc - 1 - i;

                if (cfg->cantParams > 0) {
                    cfg->params = &argv[i + 1];
                } else {
                    cfg->params = NULL;
                }

                /// Todo lo que sigue son parametros del programa
                break;
            }

            /// Flag -d de desensamblador
            else if (strcmp(argv[i], "-d") == 0) {
                cfg->flagD = 1;
            }

            /// Parametro m=M o M=M
            else if ((argv[i][0] == 'm' || argv[i][0] == 'M') && argv[i][1] == '=') {
                if (memEspecificada) {
                    reportarError(ERROR_ARGUMENTOS_INVALIDOS);
                }

                if (!parsearParametroMemoria(argv[i], &cfg->mem_kib)) {
                    reportarError(ERROR_ARGUMENTOS_INVALIDOS);
                }

                cfg->flagM = 1;
                memEspecificada = 1;
            }

            /// Archivo .vmx
            else if (tieneExtension(argv[i], ".vmx")) {
                if (cfg->archVMX != NULL) {
                    reportarError(ERROR_ARGUMENTOS_INVALIDOS);
                }

                cfg->archVMX = argv[i];
            }

            /// Archivo .vmi
            else if (tieneExtension(argv[i], ".vmi")) {
                if (cfg->archVMI != NULL) {
                    reportarError(ERROR_ARGUMENTOS_INVALIDOS);
                }

                cfg->archVMI = argv[i];
            }

            /// Argumento invalido o desconocido
            else {
                reportarError(ERROR_ARGUMENTOS_INVALIDOS);
            }
        }

        /// Al menos uno de .vmx o .vmi es obligatorio
        if (cfg->archVMX == NULL && cfg->archVMI == NULL) {
            reportarError(ERROR_ARGUMENTOS_INVALIDOS);
        }

        /// Si no se especifica .vmx, se ignoran los parametros de -p
        if (cfg->archVMX == NULL && cfg->flagP) {
            cfg->flagP = 0;
            cfg->cantParams = 0;
            cfg->params = NULL;
        }
    }
}

void mostrarConfiguracion(const ConfiguracionMV *cfg) {
    printf("Configuracion de la Maquina Virtual:\n");
    printf(" Archivo VMX    : %s\n", cfg->archVMX ? cfg->archVMX : "(no especificado)");
    printf(" Archivo VMI    : %s\n", cfg->archVMI ? cfg->archVMI : "(no especificado)");
    printf(" Memoria RAM    : %u KiB\n", cfg->mem_kib);
    printf(" Memoria indicada: %s\n", cfg->flagM ? "SI" : "NO");
    printf(" Desensamblar   : %s\n", cfg->flagD ? "SI" : "NO");
    if (cfg->flagP) {
        printf(" Parametros (-p): %d argumento(s)\n", cfg->cantParams);
        for (int i = 0; i < cfg->cantParams; i++) {
            printf("    [%d]: \"%s\"\n", i, cfg->params[i]);
        }
    } else {
        printf(" Parametros (-p): (ninguno)\n");
    }
}

void mostrarMetadatos(const ConfiguracionMV *cfg) {
    mostrarConfiguracion(cfg);
    mostrarHeader(&cfg->header);
}
