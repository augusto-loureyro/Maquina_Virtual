#include "memory.h"


int32_t cambioLogicFisic(ETMaquinaVirtual *maqVirt, int32_t dirLogica) {
    ETSegmento segmento;
    int despla = dirLogica & 0xFFFF;
    int numSeg = dirLogica >> 16;
    int32_t dirFisica;

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

uint8_t siguienteByte(ETMaquinaVirtual *maqVirt, int32_t *dirLogica) {
    uint8_t byte;
    if (*dirLogica >= DIMMEMORIA) {
        mvError(vm, "Fallo de segmento (fetch instruccion).");
        return 0;
    }else{
        byte = maqVirt->memoria[*dirLogica];
        (*dirLogica)++;
        return byte;
    }
}





/// Lectura y Escritura
int32_t readMem(ETMaquinaVirtual *maqVirt, int32_t dirLogica){
    int32_t dirFisica, valor;

    maqVirt->registros[REGLAR] = dirLogica;
    dirFisica = cambioLogicFisic(maqVirt,dirLogica);
    if (dirFisica < 0)
        mvError(maqVirt, "Fallo de segmento (lectura).");
        return 0;
    else{
        maqVirt->registros[REGMAR] = dirFisica;

        /// Lectura en Big Endian
        valor = (maqVirt->memoria[phys_addr] << 24) |
                        (maqVirt->memoria[phys_addr + 1] << 16) |
                        (maqVirt->memoria[phys_addr + 2] << 8) |
                        (maqVirt->memoria[phys_addr + 3]);

        maqVirt->registros[REGMBR] = valor;
        return valor;
    }
}
void writeMem(ETMaquinaVirtual *maqVirt, int32_t dirLogica, int32_t valor){
    int32_t dirFisica;


    maqVirt->registros[REGLAR] = dirLogica;
    dirFisica = cambioLogicFisic(maqVirt,dirLogica);
    if(dirFisica < 0)
        mvError(maqVirt, "Fallo de segmento (escritura).");
    else{
        maqVirt->registros[REGMAR] = dirFisica;
        maqVirt->registros[REGMBR] = valor;


        /// Escritura en Big Endian (4 bytes)
        maqVirt->memoria[dirFisica] = (valor >> 24) & 0xFF;
        maqVirt->memoria[dirFisica + 1] = (valor >> 16) & 0xFF;
        maqVirt->memoria[dirFisica + 2] = (valor >> 8) & 0xFF;
        maqVirt->memoria[dirFisica + 3] = valor & 0xFF;
    }
}











