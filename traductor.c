#include "traductor.h"


TRInstruction leerInstruccion(ETMaquinaVirtual *maqVirt, int32_t *dirFisica) {
    TRInstruction instancia = {0, 0, 0, 0, 0, 0};
    uint8_t byte = siguienteByte(maqVirt, dirFisica);
    uint8_t opB = byte >> 6;
    uint8_t opA = (byte >> 4) & 0x03; /// XX01 & 0011
    uint8_t oper = byte & 0x1F;
    uint8_t quinto = (byte >> 4) & 0x01;

    if (quinto == 0) {
        if (opB == 0)
            /// 0 operandos
            instancia.cantOperand = 0;
        else{
            /// 1 operando
            instancia.cantOperand = 1;
            instancia.tipoOpA = opB; /// En 1 operando, opA ocupa los primeros 2 bits
        }
        instancia.operacion = oper;
    } else {
        /// 2 operandos
        instancia.operacion = oper; ///Quinto
        instancia.cantOperand = 2;
        instancia.tipoOpB = opB;
        instancia.tipoOpA = opA;
    }

    if (instancia.cantOperand == 2) {/// Leer operando B primero
        instancia.opBValor = leerOperando(maqVirt, dirFisica, instancia.tipoOpB);
        instancia.opAValor = leerOperando(maqVirt, dirFisica, instancia.tipoOpA);
    } else if (instancia.cantOperand == 1) {
        instancia.opAValor = leerOperando(maqVirt, dirFisica, instancia.tipoOpA);
    }

    return instancia;
}

int32_t leerOperando(ETMaquinaVirtual *maqVirt, int32_t *dirFisica, uint8_t tipoOp) {
    int bytesALeer = 0, i;
    int32_t concat = 0, byte;

    /// Determinar cuantos bytes necesita el operando
    switch (tipoOp) {
        case OPNONE:
            return 0;
        case OPREG:
            bytesALeer = 1;
            break;
        case OPIMM:
            bytesALeer = 2;
            break;
        case OPMEM:
            bytesALeer = 3;
            break;
        default:
            return 0;
    }

    /// Leer secuencialmente los N bytes
    for (i = 0; i < bytesALeer; i++) {
        byte = siguienteByte(maqVirt, dirFisica);
        concat = (concat << 8) | byte;
    }
    return concat;
}



int32_t operandoDest(ETMaquinaVirtual *maqVirt, uint8_t tipoOp, int32_t valorOp, int *memoriaP) {
    uint8_t registro;
    int32_t reg_idx;
    short desplaz;

    if (tipoOp == OPREG) {
        *memoriaP = 0;
        return valorOp & 0x1F;
    } else if (tipoOp == OPMEM) {
        *memoriaP = 1;
        desplaz = (short)(valorOp >> 8);
        registro = operando & 0x1F;
        dirLogica = maqVirt->registros[registro]; /// DS
        return dirLogica + desplaz;
    }
    *memoriaP = 0;
    return 0;
}







///---------------------------------------
void llamadaSistema(ETMaquinaVirtual *maqVirt, int32_t tipoLlamada) {
    int32_t addr, count;
    int i;

    if (tipoLlamada == 0) /// FIN
        maqVirt->running = false;
    else
        if (tipoLlamada == 1){ /// READ

        }else
            if(tipoLlamada == 2){ /// WRITE
            }
}

