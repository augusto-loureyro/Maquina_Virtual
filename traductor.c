#include "traductor.h"
#include "memoriaPrincipal.h"


int32_t obtenerValorOperando(ETMaquinaVirtual *maqVirt, uint8_t tipoOp, int32_t valorOp){
    int32_t dirLogica;

    if(tipoOp == OPREG)
        /// Devuelve el contenido almacenado en el registro (ej: si es DS, devuelve 0x00010000)
        return maqVirt->registros[valorOp & 0x1F];  ///Los 5 bits menos sigficativos representan el Registro
    else
        if(tipoOp == OPMEM){
            /// Calcula la dirección logica y lee de la memoria RAM
            dirLogica = operandoDest(maqVirt, tipoOp, valorOp);
            return readMem(maqVirt, dirLogica);
        }else
            /// Inmediato (OPIMM)
            return valorOp;

}

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
    }else{
        /// 2 operandos
        instancia.cantOperand = 2;
        instancia.tipoOpB = opB;
        instancia.tipoOpA = opA;
    }
    instancia.operacion = oper; ///Quinto

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
            break;
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
    int32_t dirLogica = maqVirt->registros[REGEDX];
    int32_t regECX    = maqVirt->registros[REGECX];
    int32_t regEAX    = maqVirt->registros[REGEAX];
    int32_t dirFisica, valor;
    int i;

    /// Extraer tamaño y cantidad desde ECX
    uint16_t tamBytes = (regECX >> 16) & 0xFFFF; /// 2 bytes (LDH)
    uint16_t cantidad = regECX & 0xFFFF;        /// 2 bytes (LDL)

    if(tipoLlamada == 0x0){ /// FIN
        maqVirt->registros[REGIP] = 0xFFFFFFFF;
        maqVirt->running = false;
    }else
        if(tipoLlamada == 0x1){ /// READ
            for(i = 0; i < cantidad; i++) {
                dirFisica = cambioLogicFisic(maqVirt, dirLogica);
                if (dirFisica < 0)
                    mvError(maqVirt, "Fallo de segmento en SYS READ");
                else{
                    printf("[%04X]: ", dirFisica & 0xFFFF);

                    /// Analizar formato indicado en EAX (Bit 0 = Decimal)
                    if(regEAX & 0x01)
                        scanf("%d", &valor); /// Modo Decimal
                    else
                        if(regEAX & 0x02){
                            char c;
                            scanf(" %c", &c);
                            valor = (int32_t)c; /// Modo Carácter
                        }else
                            if(regEAX & 0x08)
                                scanf("%x", &valor); /// Modo Hexadecimal
                            else
                                scanf("%d", &valor); /// Por defecto decimal

                    writeMem(maqVirt,dirLogica,valor);
                    dirLogica += tamBytes; /// Avanza el tamaño en bytes indicado por LDH
                }
            }
        }else
            if(tipoLlamada == 0x2){ /// WRITE
                dirLogica = maqVirt->registros[REGEDX];
                for(i = 0; i < cantidad; i++) {
                    dirFisica = cambioLogicFisic(maqVirt, dirLogica);
                    if(dirFisica < 0)
                        mvError(maqVirt, "Fallo de segmento en SYS WRITE");
                    else{
                        valor = readMem(maqVirt, dirLogica);
                        printf("[%04X]: ", dirFisica & 0xFFFF);

                        /// EAX permite multiples formatos simultaneos (comprobacion por bit)
                        if (regEAX & 0x10) printf("0b... ");             /// Bit 4: Binario
                        if (regEAX & 0x08) printf("0x%X ", valor);       /// Bit 3: Hexadecimal
                        if (regEAX & 0x04) printf("0o%o ", valor);       /// Bit 2: Octal
                        if (regEAX & 0x02) printf("%c ", (valor >= 32 && valor <= 126) ? valor : '.'); /// Bit 1: Char
                        if (regEAX & 0x01) printf("%d ", valor);         /// Bit 0: Decimal

                        printf("\n");
                        dirLogica += tamBytes; /// Avanza el tamaño en bytes
                    }
                }
            }
}





