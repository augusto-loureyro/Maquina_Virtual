#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "vm.h"

#define ARCHT = 100

/// argv[0] = vmx
/// argv[1] = programa.vmx
/// argv[2] = -d   (Opcional)

int main(int argc, char *argv[]) {
    ETMaquinaVirtual maqVirt;
    char nombreArchivo[ARCHT];
    bool modoDebug = false;

    strcpy(nombreArchivo, argv[1]);

    if (strcmp(argv[2],"-d") == 0)
        modoDebug = true;

    mvInic(&maqVirt, modoDebug);
    cargarArchivo(&maqVirt, nombreArchivo);
    mvEjecutar(&maqVirt);
    return 0;
}
