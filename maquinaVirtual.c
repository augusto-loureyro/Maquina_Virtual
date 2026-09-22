#include <stdlib.h>
#include "maquinaVirtual.h"
#include "traductor.h"
#include "memoriaPrincipal.h"
#include "instruccion.h"
#include "desensamblar.h"


void mvInic(ETMaquinaVirtual *maqVirt, bool disassembler) {
    int i;

    /// Se inicializa la memoria con 0 para que no haya trash.
    memset(&(maqVirt->memoria), 0, sizeof(maqVirt->memoria));

    maqVirt->disassembler = disassembler;
    maqVirt->running = false;

    /// Inicializar los registros
    for (i = 0; i < REGISTROS; i++)
        maqVirt->registros[i] = 0;

    /// Inicializar la tabla de segmentos
    for (i = 0; i < SEGMENTOS; i++)
        maqVirt->segTabla[i] = 0xFFFFFFFF;
}

void mvError(ETMaquinaVirtual *maqVirt, char *mensaje) {
    printf("ERROR: %s\n", mensaje);
    maqVirt->running = false;
}

void setFlags(ETMaquinaVirtual *maqVirt, bool n, bool z, bool c, bool o) {
    int cc = 0;

    if(n)
        cc |= NMASK;
    if(z)
        cc |= ZMASK;
    if(c)
        cc |= CMASK;
    if(o)
        cc |= OMASK;

    maqVirt->registros[REGCC] = cc;
}

void cargarArchivo(ETMaquinaVirtual *maqVirt, char *nombreArchivo) {
    FILE *arch = NULL;
    uint8_t header[8];
    unsigned int tamanioCode;
    int i;

    arch = fopen(nombreArchivo,"rb");

    if (arch == NULL) {
        mvError(maqVirt, "Archivo no encontrado\n");
        exit(1);
    }

    /// Leer la cabecera
    if (fread(header, sizeof(header), 1, arch) != 1) {
        mvError(maqVirt, "Cabecera invalida\n");
        fclose(arch);
        exit(1);
    }

    if (memcmp(header, "VMX26", 5) != 0) { // memcmp devuelve 0 si ambos bloques de memoria son exactamente iguales en su contenido
        mvError(maqVirt, "Identificador de programa invalido\n");
        fclose(arch);
        exit(1);
    }

    if (header[5] != 1) {
        mvError(maqVirt, "Version invalida\n");
        fclose(arch);
        exit(1);
    }

    tamanioCode = header[6] << 8 | header[7];

    if (tamanioCode > DIMMEMORIA) {
        mvError(maqVirt, "Segmento de codigo no cabe en memoria\n");
        fclose(arch);
        exit(1);
    }

    /// Inicializar tabla de segmentos
    maqVirt->segTabla[0] = tamanioCode;
    maqVirt->segTabla[1] = (tamanioCode << 16) | (DIMMEMORIA - tamanioCode);

    /// Inicializar CS y DS
    /// Dir logica: 2 bytes cod. segmento, 2 bytes offset
    maqVirt->registros[REGCS] = 0x00000000;
    maqVirt->registros[REGDS] = 0x00010000;

    /// IP apunta al inicio del segmento de codigo
    maqVirt->registros[REGIP] = maqVirt->registros[REGCS];

    /// Cargar codigo/datos en memoria
    /// Header como offset
    i = 8;
    while (i-8 < DIMMEMORIA && fread(maqVirt->memoria + i, 1, 1, arch) == 1)
        i++;

    if (i-8 == DIMMEMORIA) {
        mvError(maqVirt, "Desbordamiento de memoria\n");
        fclose(arch);
        exit(1);
    }

    fclose(arch);
}

void mvEjecutar(ETMaquinaVirtual *maqVirt) {
    unsigned int dirFisica, dirFisicaTem, lengInstr;

    maqVirt->running = true;

    while (maqVirt->running) {
        if (maqVirt->registros[REGIP] == 0xFFFFFFFF) /// STOP
            maqVirt->running = false;
        else{
            dirFisica = cambioLogicFisic(maqVirt, maqVirt->registros[REGIP], sizeof(maqVirt->registros[0]));

            if (dirFisica == -1)
                mvError(maqVirt, "Fallo de segmento");
            else
            {
                dirFisicaTem = dirFisica;

                /// Actualiza OPC, OP1 y OP2
                leerInstruccion(maqVirt, &dirFisicaTem);

                /// Modo desensamblar: Muestra la instruccion
                if (maqVirt->disassembler)
                    mostrarInstruccion(maqVirt, dirFisica, dirFisicaTem);

                /// Actualizamos IP
                lengInstr = dirFisicaTem - dirFisica;
                maqVirt->registros[REGIP] += lengInstr;

                ejecutarInstruccion(maqVirt);
            }
        }
    }
}


void guardarResult(ETMaquinaVirtual *maqVirt, int tipoOp, int valorOp, int result) {
    int dest;

    if(tipoOp != OPIMM) {
        dest = operandoDest(maqVirt, tipoOp, valorOp);

        if(tipoOp == OPMEM)
            writeMem(maqVirt, dest, result, sizeof(maqVirt->registros[0]));
        else
            maqVirt->registros[dest] = result;
    }
}











