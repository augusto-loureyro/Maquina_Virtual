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