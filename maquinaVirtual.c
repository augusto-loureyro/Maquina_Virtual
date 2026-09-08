#include "maquinaVirtual.h"



void mvInic(ETMaquinaVirtual *maqVirt, bool modoDebug) {
    int i;

    /// Se inicializa la memoria con 0 para que no haya trash.
    memset(&(maqVirt->memoria), 0, sizeof(maqVirt->memoria));

    /// Se inicializan variables de control
    maqVirt->modoDebug = modoDebug;
    maqVirt->error = false;
    maqVirt->running = false;

    /// Inicializar los 32 registros
    for (i = 0; i < REGISTROS; i++)
        maqVirt->registros[i] = 0;

    /// Inicializar las tablas de los 8 segmentos
    for (i = 0; i < SEGMENTOS; i++) {
        maqVirt->segTabla[i].base = 0xFFFF;
        maqVirt->segTabla[i].dim = 0xFFFF;
    }
}

void mvError(ETMaquinaVirtual *maqVirt,char *mensaje) {
    printf("ERROR: %s\n", mensaje);
    maqVirt->error = true;
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

bool cargarArchivo(ETMaquinaVirtual *maqVirt, char *nombreArchivo) {
    FILE *arch = NULL;
    char header[8];
    int tamanioCode;
    bool flag = false;

    *arch = fopen(nombreArchivo, "rb");
    if (arch != NULL) {
        /// Leer la cabecera
        if (fread(header, sizeof(header), 1, arch) != 1) {
            printf("Cabecera invalida\n");
        }else
            if (memcmp(header, "VMX26", 5) != 0) { ///memcmp. Devuelve 0 si ambos bloques de memoria son exactamente iguales en su contenido.
                printf("Identificador de programa invalido\n");
            }else
                if (header[5] != 1)
                    printf("Version invalida\n");
                else{
                    /// ShiftL 8 bits del  primero y concateno con los del segundo
                    tamanioCode = (header[6] << 8) | header[7];

                    /// Inicializar tabla de segmentos
                    /// Segmento de Codigo
                    maqVirt->segTabla[0].base = 0;
                    maqVirt->segTabla[0].dim = tamanioCode;

                    /// Segmento de Datos
                    maqVirt->segTabla[1].base = tamanioCode;
                    maqVirt->segTabla[1].dim = DIMMEMORIA - tamanioCode;

                    /// Inicializar CS y DS
                    maqVirt->registros[REGCS] = 0x00000000;
                    maqVirt->registros[REGDS] = 0x00010000;

                    /// IP apunta al inicio del segmento de código
                    maqVirt->registros[REGIP] = maqVirt->registros[REGCS];

                    /// Cargar codigo en memoria física
                    fread(maqVirt->memoria, 1, DIMMEMORIA, arch);
                    fclose(arch);
                    flag = true;
                }
        fclose(arch);
        return flag;
    }else{
        printf("No se pudo abrir el archivo: %s\n", nombreArchivo);
        return flag;
    }
}

void mvEjecutar(ETMaquinaVirtual *maqVirt) {
    maqVirt->running = true;
    int dirLogica, dirFisica, dirFisicaTem;

    while (maqVirt->running && !(maqVirt->error)) {
        if (maqVirt->registros[REGIP] == 0xFFFFFFFF) {
            maqVirt->running = false; /// Detiene ejecucion.
        }else{
            dirLogica = maqVirt->registros[REGIP];
            dirFisica = cambioLogicFisic(maqVirt, dirLogica);

            if (dirFisica >= 0) {
                dirFisicaTem = dirFisica;

                // Decodificamos la instrucción leyendo desde la memoria física
                //Instruction inst = leerInstruccion(maqVirt, &dirFisicaTem);




                maqVirt->registros[REGOPC] =
                maqVirt->registros[REGOP1] =
                maqVirt->registros[REGOP2] =

            }else
                maqVirt->running = false; /// Fuera del segmento de código
        }
    }
}
