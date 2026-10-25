#include "registros.h"

void inicializarRegistros(Registros r,IndiceSgm index){
    for (int i = 0; i < CANT_REGISTROS; i++){
        r[i] = 0;
    }

    // CS y DS: 16 bits altos = número de segmento en la tabla de descriptores, 16 bits bajos = 0
    r[REGCS] = (uint32_t)index.cs << 16;
    r[REGDS] = (uint32_t)index.ds << 16;

    // IP apunta a la primera instrucción del segmento de código: mismo valor que CS
    r[REGIP] = r[REGCS];
}