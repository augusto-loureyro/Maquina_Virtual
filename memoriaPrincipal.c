#include "memoriaPrincipal.h"

unsigned int cambioLogicFisic(ETMaquinaVirtual *maqVirt, unsigned int dirLogica, unsigned int tamBytes) {
    unsigned int segmento, dimSeg, dirFisica, numSeg = dirLogica >> 16;
    int baseSeg, despla = dirLogica & 0xFFFF;

    if (numSeg < 0 || numSeg > SEGMENTOS) ///Segmento inexistente
        return -1;

    segmento = maqVirt->segTabla[numSeg];

    if (segmento == 0xFFFFFFFF) /// Segmento Restringido
        return -1;

    baseSeg = segmento >> 16;
    dimSeg = segmento & 0xFFFF;
    dirFisica = baseSeg + despla;

    if (tamBytes > dimSeg || dirFisica < baseSeg || dirFisica - baseSeg > dimSeg - tamBytes) /// Desbordamiento de segmento
        return -1;
    else
        return dirFisica;
}


/// Lectura y Escritura
int8_t leerByteDirFisica(ETMaquinaVirtual *maqVirt, unsigned int dirFisica) {
    if (dirFisica >= DIMMEMORIA || dirFisica < 0) {
        mvError(maqVirt, "Direccion invalida");
        return 0;
    }

    return maqVirt->memoria[dirFisica];
}


int readMem(ETMaquinaVirtual *maqVirt, unsigned int dirLogica, unsigned int tamBytes) {
    unsigned int dirFisica;
    int i, valor = 0;

    maqVirt->registros[REGLAR] = dirLogica;

    dirFisica = cambioLogicFisic(maqVirt,dirLogica,tamBytes);

    if (dirFisica == -1)
        mvError(maqVirt, "Fallo de segmento (lectura).");
    else {
        maqVirt->registros[REGMAR] = dirFisica;

        /// Lectura en Big Endian
        for (i = 0; i < tamBytes; i++)
            valor = (valor << 8) | (uint8_t)maqVirt->memoria[dirFisica + i];

        /*valor = (maqVirt->memoria[dirFisica] << 24) |
                (maqVirt->memoria[dirFisica + 1] << 16) |
                (maqVirt->memoria[dirFisica + 2] << 8) |
                (maqVirt->memoria[dirFisica + 3]);*/

        maqVirt->registros[REGMBR] = valor;
    }

    return valor;
}


void writeMem(ETMaquinaVirtual *maqVirt, unsigned int dirLogica, int valor, unsigned int tamBytes) {
    unsigned int dirFisica;
    int i;

    maqVirt->registros[REGLAR] = dirLogica;

    dirFisica = cambioLogicFisic(maqVirt,dirLogica,tamBytes);

    if(dirFisica == -1)
        mvError(maqVirt, "Fallo de segmento (escritura).");
    else {
        maqVirt->registros[REGMAR] = dirFisica;
        maqVirt->registros[REGMBR] = valor;

        /// Escritura en Big Endian
        for (i = tamBytes - 1; i >= 0; i--) {
            maqVirt->memoria[dirFisica + i] = valor & 0xFF;
            valor >>= 8;
        }

        /*maqVirt->memoria[dirFisica] = (valor >> 24) & 0xFF;
        maqVirt->memoria[dirFisica + 1] = (valor >> 16) & 0xFF;
        maqVirt->memoria[dirFisica + 2] = (valor >> 8) & 0xFF;
        maqVirt->memoria[dirFisica + 3] = valor & 0xFF;*/
    }
}
