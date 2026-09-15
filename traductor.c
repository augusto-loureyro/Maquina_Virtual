#include "traductor.h"
#include "memoriaPrincipal.h"


TRInstruction leerInstruccion(ETMaquinaVirtual *maqVirt, int32_t *dirFisica) {
    TRInstruction instancia = {0, 0, 0, 0, 0, 0};
    uint8_t byte = siguienteByte(maqVirt, dirFisica);
    uint8_t opB = byte >> 6;
    uint8_t opA = (byte >> 4) & 0x03; /// XX01 & 0011
    uint8_t oper = byte & 0x1F;
    uint8_t quinto = (byte >> 4) & 0x01;

    if(quinto == 0){
        if(opB == 0)
            /// 0 operandos
            instancia.cantOperand = 0;
        else{
            /// 1 operando
            instancia.cantOperand = 1;
            instancia.tipoOpA = opB; /// En 1 operando, opA ocupa los primeros 2 bits
        }
        instancia.operacion = oper;
    }else{
        /// 2 operandos
        instancia.operacion = oper; ///Quinto
        instancia.cantOperand = 2;
        instancia.tipoOpB = opB;
        instancia.tipoOpA = opA;
    }

    if(instancia.cantOperand == 2){/// Leer operando B primero
        instancia.opBValor = leerOperando(maqVirt, dirFisica, instancia.tipoOpB);
        instancia.opAValor = leerOperando(maqVirt, dirFisica, instancia.tipoOpA);
    }else
        if(instancia.cantOperand == 1)
            instancia.opAValor = leerOperando(maqVirt, dirFisica, instancia.tipoOpA);

    return instancia;
}

int32_t leerOperando(ETMaquinaVirtual *maqVirt, int32_t *dirFisica, uint8_t tipoOp) {
    int bytesALeer = 0, i;
    int32_t concat = 0, byte;

    /// Determinar cuantos bytes necesita el operando
    switch (tipoOp) {
        case OPNONE:
            bytesALeer = 0;
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
            bytesALeer = 0;
    }

    /// Leer secuencialmente los N bytes
    for(i = 0; i < bytesALeer; i++){
        byte = siguienteByte(maqVirt, dirFisica);
        concat = (concat << 8) | byte;
    }
    return concat;
}



int32_t operandoDest(ETMaquinaVirtual *maqVirt, uint8_t tipoOp, int32_t valorOp) {
    uint8_t registro;
    short desplaz;
    int32_t dirLogica;

    if(tipoOp == OPREG){
        return valorOp & 0x1F;
    }else
        if(tipoOp == OPMEM){
            desplaz = (short)(valorOp >> 8);
            registro = valorOp & 0x1F;
            dirLogica = maqVirt->registros[registro]; /// DS
            return dirLogica + desplaz;
        }else
            return 0;
}


void llamadaSistema(ETMaquinaVirtual *maqVirt, int32_t tipoLlamada) {
    int32_t dirLogica, dirFisica, cant, valor;
    int i;

    if(tipoLlamada == 0x0){ /// FIN
        maqVirt->registros[REGIP] = 0xFFFFFFFF;
        maqVirt->running = false;
    }else
        if(tipoLlamada == 0x1){ /// READ
            dirLogica = maqVirt->registros[REGEDX];
            cant = maqVirt->registros[REGECX] & 0xFFFF;

            for(i = 0; i < cant; i++) {
                dirFisica = cambioLogicFisic(maqVirt, dirLogica);
                if (dirFisica < 0)
                    mvError(maqVirt, "Fallo de segmento en SYS READ");
                else{
                    printf("[%04X]: ", dirFisica & 0xFFFF);
                    if(scanf("%d", &valor) != 1)
                        valor = 0;
                    writeMem(maqVirt,dirLogica,valor);
                }
            }
        }else
            if(tipoLlamada == 0x2){ /// WRITE
                dirLogica = maqVirt->registros[REGEDX];
                cant = maqVirt->registros[REGECX] & 0xFFFF;
                for(i = 0; i < cant; i++) {
                    dirFisica = cambioLogicFisic(maqVirt, dirLogica);
                    if(dirFisica < 0)
                        mvError(maqVirt, "Fallo de segmento en SYS WRITE");
                    else{
                        valor = readMem(maqVirt, dirLogica);
                        printf("[%04X]: %d\n", dirFisica & 0xFFFF, valor);
                    }
                }
            }
}





