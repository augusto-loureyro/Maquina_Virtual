#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "consola.h"
#include "errores.h"

static int tieneExtensionVmx(const char *nombre) {
    size_t len = strlen(nombre);
    if (len < 4)
        return 0;
    else{
        const char *ext = nombre + len - 4;
        return (ext[0] == '.' && tolower((unsigned char)ext[1]) == 'v' && tolower((unsigned char)ext[2]) == 'm' && tolower((unsigned char)ext[3]) == 'x');
    }
}

void parsearArgumentos(int argc, char *argv[], char **nombreArchivo, int *flagD) {
    *nombreArchivo = NULL;
    *flagD = 0;

    if (argc < 2) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            *flagD = 1;
        } else if (tieneExtensionVmx(argv[i])) {
            *nombreArchivo = argv[i];
        } else if (i == 1 && strcmp(argv[i], "vmx") == 0) {
            // Tolera cuando el entorno (ej. parámetros en Code::Blocks) incluye "vmx" como argv[1]
            continue;
        } else {
            reportarError(ERROR_ARGUMENTOS_INVALIDOS);
        }
    }

    if (*nombreArchivo == NULL) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }
}
