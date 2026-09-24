#include "traductor.h"
#include "memoriaPrincipal.h"

void leerInstruccion(ETMaquinaVirtual *maqVirt, unsigned int *dirFisica) {
    uint8_t byte = (uint8_t)leerByteDirFisica(maqVirt, *dirFisica);
    uint8_t bit7y6 = byte >> 6;
    uint8_t bit5y4 = byte >> 4 & 0x03; /// XX01 & 0011
    uint8_t bit4a0 = byte & 0x1F;
    //printf("%X %X %X %X\n", byte, bit7y6, bit5y4, bit4a0);

    (*dirFisica)++;
    maqVirt->registros[REGOPC] = bit4a0;

    if((bit4a0 & 0xF0) == 0) { /// bit 4 en 0 es codigo operacion para 0/1 operandos
        maqVirt->registros[REGOP2] = 0;
        /// 0 operandos
        if(bit7y6 == 0)
            maqVirt->registros[REGOP1] = 0;
        /// 1 operando
        else
            maqVirt->registros[REGOP1] = bit7y6 << 24 | leerOperando(maqVirt, dirFisica, bit7y6);
    }else {
        /// 2 operandos
        maqVirt->registros[REGOP2] = bit7y6 << 24 | leerOperando(maqVirt, dirFisica, bit7y6);
        maqVirt->registros[REGOP1] = bit5y4 << 24 | leerOperando(maqVirt, dirFisica, bit5y4);
    }

    //printf("%X ", maqVirt->registros[REGOP2]);
    //printf("%X \n", maqVirt->registros[REGOP1]);
}

int leerOperando(ETMaquinaVirtual *maqVirt, unsigned int *dirFisica, unsigned int bytesALeer) {
    int i, operando = 0;
    uint8_t byte;

    for (i = 0; i < bytesALeer; i++) {
        byte = leerByteDirFisica(maqVirt, *dirFisica);
        (*dirFisica)++;
        operando = (operando << 8) | byte;
    }

    return operando;
}

int obtenerValorOperando(ETMaquinaVirtual *maqVirt, int tipoOp, int op) {
    if(tipoOp == OPREG)
        /// Devuelve el contenido almacenado en el registro (ej: si es DS, devuelve 0x00010000)
        return maqVirt->registros[op & 0x1F];  ///Los 5 bits menos sigficativos representan el Registro
    else
        if(tipoOp == OPMEM)
            /// Lee de la memoria RAM en la direccion logica del operando
            return readMem(maqVirt, operandoDest(maqVirt, tipoOp, op), sizeof(maqVirt->registros[0]));
        else
            /// Inmediato
            return (int16_t)(op & 0xFFFF);

}

int operandoDest(ETMaquinaVirtual *maqVirt, int tipoOp, int op) {
    unsigned int registro, dirLogica;
    int desplaz;

    if(tipoOp == OPREG) {
        /// Devuelve num de registro
        return op & 0x1F;
    }else
        if(tipoOp == OPMEM) {
            /// Interpreta y devuelve direccion logica
            desplaz = (op >> 8) & 0xFFFF;
            registro = op & 0x1F;
            dirLogica = maqVirt->registros[registro] + desplaz;
            //printf("\nregistro %d  desplazamiento %d  dirLogica %X\n", registro, desplaz, dirLogica);
            return dirLogica;
        }else
            /// Inmediato
            return op & 0xFFFF;
}

void llamadaSistema(ETMaquinaVirtual *maqVirt, int tipoLlamada) {
    unsigned int dirLogica = (unsigned int)maqVirt->registros[REGEDX];
    unsigned int modo = (unsigned int)maqVirt->registros[REGEAX];
    unsigned int regECX = (unsigned int)maqVirt->registros[REGECX];
    unsigned int dirFisica;
    int i, j, valor = 0;
    char binario[17] = {0};
    /// Extraer tamaño y cantidad desde ECX
    unsigned int tamBytes = regECX >> 16; /// LDH
    unsigned int cant = regECX & 0xFFFF; /// LDL

    //printf("\nllamada tipo %d\n", tipoLlamada);
    dirFisica = cambioLogicFisic(maqVirt, dirLogica, tamBytes*cant);

    if (dirFisica == -1) {
        mvError(maqVirt, "Fallo de segmento en SYS");
        return;
    }

    /// READ
    if(tipoLlamada == 0x1) {
        for(i = 0; i < cant; i++) {
            printf("[%04X]: ", dirFisica);

            /// Analizar formato indicado en EAX
            switch(modo) {
                case 0x01:
                    /// Modo Decimal
                    scanf("%d", &valor);
                    break;
                case 0x02:
                    /// Modo Caracter
                    scanf("%c", &valor);
                    break;
                case 0x04:
                    /// Modo Octal
                    scanf("%o", &valor);
                    break;
                case 0x08:
                    /// Modo Hexadecimal
                    scanf("%x", &valor);
                    break;
                case 0x10:
                    /// Modo Binario (convertido desde string)
                    scanf("%s", binario);
                    for(j = 0; binario[j] != '\0'; j++)
                        valor = valor*2 + binario[j] - '0'; /// agregar cifra derecha => num*base + cifra
                    break;
                default:
                    mvError(maqVirt, "Modo de lectura invalido");
                    return;
            }

            writeMem(maqVirt,dirLogica,valor,tamBytes);

            /// Avanza el tamaño en bytes indicado por LDH ECX
            dirLogica += tamBytes;
            dirFisica += tamBytes;
        }
    }else
        if(tipoLlamada == 0x2) { /// WRITE
            for(i = 0; i < cant; i++) {
                valor = readMem(maqVirt, dirLogica, tamBytes);

                printf("[%04X]: ", dirFisica);

                    /// EAX permite multiples formatos simultaneos (comprobacion por bit)
                    /// Bit 4: Binario
                    if (modo & 0x10) {
                        printf("0b");
                        for (j = 15; j >= 0; j--) /// imprime bit a bit
                            printf("%d", (valor >> j) & 1);
                        printf(" ");
                    }
                    /// Bit 3: Hexadecimal
                    if (modo & 0x08) printf("0x%X", valor);
                    /// Bit 2: Octal
                    if (modo & 0x04) printf("0o%o", valor);
                    /// Bit 1: Char
                    if (modo & 0x02) printf("%c", (valor >= 32 && valor <= 126) ? valor : '.');
                    /// Bit 0: Decimal
                    if (modo & 0x01) printf("%d", valor);

                    printf("\n");

                    dirLogica += tamBytes;
                    dirFisica += tamBytes;
                }
        } else
            mvError(maqVirt, "Argumento invalido para SYS (1: lectura - 2: escritura)");
}
