#include "memory.h"


int cambioLogicFisic(ETMaquinaVirtual *maqVirt, int dirLogica) {
    ETSegmento segmento;
    int despla = dirLogica & 0xFFFF;
    int numSeg = dirLogica >> 16;
    int dirFisica;

    if (numSeg >= 0 && numSeg <= SEGMENTOS ) {
        segmento = maqVirt->segTabla[numSeg];
        dirFisica = segmento.base + despla;

        if (segmento.base == 0xFFFF && segmento.dim == 0xFFFF) {
            return -1; /// Segmento Restringido
        }else{
            dirFisica = segmento.base + despla;
            if(dirFisica < segmento.dim)
                return dirFisica;
            else
                return -1; /// Overflow del segmento
        }
    }else
        return -1; /// Segmento Inexistente
}



char siguienteInstruc(ETMaquinaVirtual *maqVirt, int *dirLogica) {
    char byte = maqVirt->memoria[*dirLogica];
    (*dirLogica)++;
    return byte;
}
