#include "registros.h"
#include "tablaSegmentos.h"

void inicializarRegistros(Registros r) {
    for (int i = 0; i < CANT_REGISTROS; i++) {
        r[i] = 0;
    }

    /// CS y DS: 16 bits altos = numero de segmento en la tabla, 16 bits bajos = 0
    r[REGCS] = (uint32_t)CODE << 16;
    r[REGDS] = (uint32_t)DATA << 16;

    /// IP apunta a la primera instruccion dentro del segmento de codigo (mismo valor inicial que CS)
    r[REGIP] = r[REGCS];
}
