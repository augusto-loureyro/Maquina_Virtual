#include "../include/registros.h"

void inicializarRegistros(Registros r){
    for (int i = 0; i < CANT_REGISTROS; i++){
        r[i] = 0;
    }

    //de CS y DS, el offset es 0 (queda igual, parte baja), la parte alta es el num de segmento
    r[REGCS] = LCH(r[REGCS],CODE); 
    r[REGDS] = LCH(r[REGCS],DATA);
    r[REGIP] = r[REGCS]; //IP apunta a la primera instruccion del CodeSegment
}

