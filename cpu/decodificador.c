#include "decodificador.h"

void decodificarPrimerByte(uint8_t primerByte, int *opcode, int *categoria) {
    *opcode = primerByte & 0x1F; // 5 bits menos significativos

    if (*opcode == 0x0F) {
        *categoria = SIN_OPERANDOS;
    } else if (*opcode <= 0x0A) {
        *categoria = UN_OPERANDO;
    } else if (*opcode >= 0x10 && *opcode <= 0x1F) {
        *categoria = DOS_OPERANDOS;
    } else {
        reportarError(ERROR_INSTRUCCION_INVALIDA);
    }
}

void extraerTiposOperando(uint8_t primerByte, int categoria, int *tipoA, int *tipoB) {
    switch (categoria) {
        case DOS_OPERANDOS:
            // En lenguaje maquina, el operando B va primero en los bits 7-6,
            // y el operando A en los bits 5-4.
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

int tamanioOperando(int tipo) {
    /// Por especificación: el tamaño en bytes coincide con el codigo binario del tipo
    /// (0: 0 bytes, 1: 1 byte, 2: 2 bytes, 3: 3 bytes)
    return tipo;
}

Operando leerOperando(int tipo, uint8_t *bytes) {
    Operando o;
    o.tipo = tipo;
    o.codigoRegistro = 0;
    o.desplazamiento = 0;
    o.inmediato = 0;

    switch (tipo) {
        case TIPO_REGISTRO:
            /// 1 byte: 3 bits reservados + 5 bits codigo de registro
            o.codigoRegistro = bytes[0] & 0x1F;
            break;

        case TIPO_INMEDIATO:
            /// 2 bytes: entero de 16 bits con signo (big-endian)
            o.inmediato = (int16_t)((bytes[0] << 8) | bytes[1]);
            break;

        case TIPO_MEMORIA:
            /// 3 bytes: 2 bytes offset (big-endian) + 1 byte (3 bits reservados + 5 bits registro)
            o.desplazamiento = (int16_t)((bytes[0] << 8) | bytes[1]);
            o.codigoRegistro = bytes[2] & 0x1F;
            break;

        case TIPO_NINGUNO:
        default:
            break;
    }

    return o;
}
