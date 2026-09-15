#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> ///Para la funcion RND
#include "maquinaVirtual.h"

#define ARCHT 100

/// argv[0] = vmx
/// argv[1] = programa.vmx
/// argv[2] = -d   (Opcional)

int main(int argc, char *argv[]) {
    ETMaquinaVirtual maqVirt;
    char nombreArchivo[ARCHT];
    bool modoDisamble = false;
    srand(time(NULL));

    if (argc < 2) {
        printf("Uso: vmx filename.vmx [-d]\n");
        return 1;
    }else{
        strcpy(nombreArchivo, argv[2]);

        if (argc >= 3 && strcmp(argv[3], "-d") == 0)
            modoDisamble = true;

        mvInic(&maqVirt, modoDisamble);

        if (!cargarArchivo(&maqVirt, nombreArchivo)) {
            return 1;
        }else{
            mvEjecutar(&maqVirt);
            return 0;
        }
    }
}
