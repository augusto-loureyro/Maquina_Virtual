#include "consola.h"
#include "errores.h"
#include <string.h>

void parsearArgumentos(int argc, char *argv[], char **nombreArchivo,int *flagD){
    int i; 

    if (argc < 2) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    *nombreArchivo = argv[1];

    if (strstr(*nombreArchivo, ".vmx") == NULL) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    *flagD = 0;

    for (i=2; i<argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            *flagD = 1;
        } else {
            reportarError(ERROR_ARGUMENTOS_INVALIDOS);
        }
    }

}