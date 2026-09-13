#include "decodificador.h"

void decodificarPrimerByte(uint8_t primerByte, int *opcode, int *categoria){
    *opcode = primerByte & 0x1F; // 5 bits menos significativos

    if (*opcode == 0x0F){
        *categoria = SIN_OPERANDOS;
    } else if (*opcode <= 0x0A){
        *categoria = UN_OPERANDO;
    } else if (*opcode >= 0x10 && *opcode <= 0x1F){
        *categoria = DOS_OPERANDOS;
    } else {
        reportarError(ERROR_INSTRUCCION_INVALIDA);
    }
}

void extraerTiposOperando(uint8_t primerByte, int categoria, int *tipoA, int *tipoB){
    switch (categoria){
        case DOS_OPERANDOS:
            *tipoB = (primerByte >> 6) & 0x03;
            *tipoA = (primerByte >> 4) & 0x03;
            break;
        case UN_OPERANDO:
            *tipoA = (primerByte >> 6) & 0x03;
            *tipoB = TIPO_NINGUNO;
            break;
        case SIN_OPERANDOS:
            *tipoA = TIPO_NINGUNO;
            *tipoB = TIPO_NINGUNO;
            break;
    }
}

// NOTA: el tamaño del operando en bytes coincide con su correspondiente código binario.

int tamanioOperando(int tipo){
    return tipo;
}

Operando leerOperando(int tipo, uint8_t *bytes){
    Operando o;
    o.tipo = tipo;

    switch (tipo){
        case TIPO_REGISTRO:
            o.codigoRegistro = bytes[0] & 0x1F;
            break;
        case TIPO_INMEDIATO:
            o.inmediato = (int16_t)((bytes[0] << 8) | bytes[1]);
            break;
        case TIPO_MEMORIA:
            o.desplazamiento = (int16_t)((bytes[0] << 8) | bytes[1]);
            o.codigoRegistro = bytes[2] & 0x1F;
            break;
        case TIPO_NINGUNO:
            break;
    }

    return o;
}