// básico: comprueba que el comando sea válido y obtiene las flags enviadas

#include "consola.h"
#include "errores.h"


void parsearArgumentos(int argc, char *argv[], char **nombreArchivo,int *flagD){
    int i; 

    if (argc < 2) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    *nombreArchivo = argv[1];

    /*if (strstr(*nombreArchivo, ".vmx") == NULL) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }*/
    size_t len = strlen(*nombreArchivo);
    if (len < 4 || strcmp(*nombreArchivo + len - 4, ".vmx") != 0) {
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