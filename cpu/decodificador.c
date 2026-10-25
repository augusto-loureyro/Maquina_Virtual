#include "decodificador.h"
#include "opcode.h"

void decodificarPrimerByte(uint8_t primerByte, int *opcode, int *categoria) {
    *opcode = primerByte & 0x1F; // 5 bits menos significativos

    if (*opcode == OPCODE_STOP || *opcode == OPCODE_RET) {
        *categoria = SIN_OPERANDOS;
    } else if (*opcode <= OPCODE_CALL) {
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
    o.sector = SECTOR_COMPLETO;
    o.tamanoMemoria = 4;
    o.modifMemoria = MODIF_MEM_DEFAULT;
    o.desplazamiento = 0;
    o.inmediato = 0;

    switch (tipo) {
        case TIPO_REGISTRO:
            /// 1 byte: 1 bit reservado (bit 7) + 2 bits sector (bits 6-5) + 5 bits codigo de registro (bits 4-0)
            o.sector = (bytes[0] >> 5) & 0x03;
            o.codigoRegistro = bytes[0] & 0x1F;
            break;

        case TIPO_INMEDIATO:
            /// 2 bytes: entero de 16 bits con signo (big-endian)
            o.inmediato = (int16_t)((bytes[0] << 8) | bytes[1]);
            break;

        case TIPO_MEMORIA: {
            /// 3 bytes: 2 bytes offset (big-endian) + 1 byte (3 bits cant bytes + 5 bits registro)
            o.desplazamiento = (int16_t)((bytes[0] << 8) | bytes[1]);
            o.codigoRegistro = bytes[2] & 0x1F;
            o.modifMemoria = (bytes[2] >> 5) & 0x07;
            switch (o.modifMemoria) {
                case MODIF_MEM_BYTE:    // 001: 1 byte (b[])
                    o.tamanoMemoria = 1;
                    break;
                case MODIF_MEM_WORD:    // 010: 2 bytes (w[])
                    o.tamanoMemoria = 2;
                    break;
                case MODIF_MEM_LONG:    // 100: 4 bytes (l[])
                case MODIF_MEM_DEFAULT: // 000: 4 bytes (compatibilidad MV1 y [])
                    o.tamanoMemoria = 4;
                    break;
                default:
                    reportarError(ERROR_INSTRUCCION_INVALIDA);
                    break;
            }
            break;
        }

        case TIPO_NINGUNO:
        default:
            break;
    }

    return o;
}

int obtenerTamanoOperando(Operando o) {
    switch (o.tipo) {
        case TIPO_REGISTRO:
            switch (o.sector) {
                case SECTOR_BYTE_BAJO:
                case SECTOR_BYTE_ALTO:
                    return 1;
                case SECTOR_WORD_BAJO:
                    return 2;
                case SECTOR_COMPLETO:
                default:
                    return 4;
            }
        case TIPO_MEMORIA:
            return o.tamanoMemoria;
        case TIPO_INMEDIATO:
            return 2;
        default:
            return 4;
    }
}

uint32_t truncarYExtenderSigno(uint32_t val, int tamanoBytes) {
    if (tamanoBytes == 1) {
        return (uint32_t)(int32_t)(int8_t)(val & 0xFF);
    } else if (tamanoBytes == 2) {
        return (uint32_t)(int32_t)(int16_t)(val & 0xFFFF);
    }
    return val;
}
