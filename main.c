#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> /// Para settear semilla
#include "maquinaVirtual.h"

#define ARCHT 200

/// argv[0] = vmx
/// argv[1] = programa.vmx
/// argv[2] = -d   (Opcional)

int main(int argc, char *argv[]) {
    ETMaquinaVirtual maqVirt;
    char nombreArchivo[ARCHT];
    bool disassembler = false;
    unsigned int tamanioCode;
    srand(time(NULL));

    if (argc < 2) {
        printf("Uso: vmx filename.vmx [-d]\n");
        return 1;
    }

    strcpy(nombreArchivo, argv[2]);

    if (argc >= 3 && strcmp(argv[3], "-d") == 0)
        disassembler = true;

    mvInic(&maqVirt, disassembler);

    cargarArchivo(&maqVirt, nombreArchivo, &tamanioCode);

    mvEjecutar(&maqVirt, tamanioCode);

    return 0;
}
