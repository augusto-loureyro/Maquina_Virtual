#include "alu.h"
#include "errores.h"



void actualizarCC(Registros registros, int flagN, int flagZ, int flagC, int flagV) {
    uint32_t cc = 0;

    if (flagN) cc |= (1u << CC_BIT_N);
    if (flagZ) cc |= (1u << CC_BIT_Z);
    if (flagC) cc |= (1u << CC_BIT_C);
    if (flagV) cc |= (1u << CC_BIT_V);

    registros[REGCC] = cc;
}

void actualizarFlagsValor(uint32_t valor, Registros registros) {
    int flagN = (int32_t)valor < 0;
    int flagZ = (valor == 0);

    actualizarCC(registros, flagN, flagZ, 0, 0);
}

/// Operaciones Aritmeticas

uint32_t sumar(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint64_t sumaCompleta = (uint64_t)operandoA + (uint64_t)operandoB;
    uint32_t resultado = (uint32_t)sumaCompleta;

    int signoA = (int32_t)operandoA < 0;
    int signoB = (int32_t)operandoB < 0;
    int signoResultado = (int32_t)resultado < 0;

    int flagN = signoResultado;
    int flagZ = (resultado == 0);
    int flagC = (sumaCompleta > 0xFFFFFFFFu);
    int flagV = (signoA == signoB) && (signoResultado != signoA);

    actualizarCC(registros, flagN, flagZ, flagC, flagV);
    return resultado;
}

uint32_t restar(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint64_t restaCompleta = (uint64_t)operandoA + (uint64_t)(~operandoB) + 1ULL;
    uint32_t resultado = (uint32_t)restaCompleta;

    int signoA = (int32_t)operandoA < 0;
    int signoB = (int32_t)operandoB < 0;
    int signoResultado = (int32_t)resultado < 0;

    int flagN = signoResultado;
    int flagZ = (resultado == 0);
    int flagC = (restaCompleta > 0xFFFFFFFFu);
    int flagV = (signoA != signoB) && (signoResultado != signoA);

    actualizarCC(registros, flagN, flagZ, flagC, flagV);
    return resultado;
}

uint32_t multiplicar(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint64_t productoSinSigno = (uint64_t)operandoA * (uint64_t)operandoB;
    int64_t productoConSigno = (int64_t)(int32_t)operandoA * (int64_t)(int32_t)operandoB;
    uint32_t resultado = (uint32_t)productoSinSigno;

    int flagN = (int32_t)resultado < 0;
    int flagZ = (resultado == 0);
    int flagC = (productoSinSigno > 0xFFFFFFFFu);
    int flagV = (productoConSigno < INT32_MIN) || (productoConSigno > INT32_MAX);

    actualizarCC(registros, flagN, flagZ, flagC, flagV);
    return resultado;
}

uint32_t dividir(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    if (operandoB == 0) {
        reportarError(ERROR_DIVISION_POR_CERO);
    }

    int overflow = ((int32_t)operandoA == INT32_MIN) && ((int32_t)operandoB == -1);

    int32_t cociente, resto;
    if (overflow) {
        cociente = INT32_MIN;
        resto = 0;
    } else {
        cociente = (int32_t)operandoA / (int32_t)operandoB;
        resto = (int32_t)operandoA % (int32_t)operandoB;
    }

    /// Por especificacion: DIV guarda el resto (modulo) en el registro AC
    registros[REGAC] = (uint32_t)resto;

    int flagN = (cociente < 0);
    int flagZ = (cociente == 0);
    int flagC = 0;
    int flagV = overflow;

    actualizarCC(registros, flagN, flagZ, flagC, flagV);
    return (uint32_t)cociente;
}

/// Operaciones Logicas

uint32_t and_logico(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint32_t resultado = operandoA & operandoB;
    actualizarCC(registros, (int32_t)resultado < 0, resultado == 0, 0, 0);
    return resultado;
}

uint32_t or_logico(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint32_t resultado = operandoA | operandoB;
    actualizarCC(registros, (int32_t)resultado < 0, resultado == 0, 0, 0);
    return resultado;
}

uint32_t xor_logico(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint32_t resultado = operandoA ^ operandoB;
    actualizarCC(registros, (int32_t)resultado < 0, resultado == 0, 0, 0);
    return resultado;
}

uint32_t not_logico(uint32_t operandoA, Registros registros) {
    uint32_t resultado = ~operandoA;
    actualizarCC(registros, (int32_t)resultado < 0, resultado == 0, 0, 0);
    return resultado;
}

/// Desplazamientos

uint32_t shift_izq(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint32_t resultado;
    int flagC = 0, flagV = 0;

    if (operandoB == 0) {
        resultado = operandoA;
    } else if (operandoB < 32) {
        resultado = operandoA << operandoB;
        uint32_t bitsQueSalen = operandoA & (0xFFFFFFFFu << (32 - operandoB));
        flagC = (bitsQueSalen != 0);

        int64_t exacto = (int64_t)(int32_t)operandoA * (1LL << operandoB);
        flagV = (exacto != (int64_t)(int32_t)resultado);
    } else {
        resultado = 0;
        flagC = (operandoB == 32) ? (operandoA & 1u) : 0;
        flagV = (operandoA != 0);
    }

    actualizarCC(registros, (int32_t)resultado < 0, resultado == 0, flagC, flagV);
    return resultado;
}

uint32_t shift_der(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint32_t resultado;
    int flagC = 0;

    if (operandoB == 0) {
        resultado = operandoA;
    } else if (operandoB < 32) {
        resultado = operandoA >> operandoB;
        flagC = (operandoA >> (operandoB - 1)) & 1u;
    } else {
        resultado = 0;
        flagC = (operandoB == 32) ? ((operandoA >> 31) & 1u) : 0;
    }

    actualizarCC(registros, (int32_t)resultado < 0, resultado == 0, flagC, 0);
    return resultado;
}

uint32_t shift_der_aritmetico(uint32_t operandoA, uint32_t operandoB, Registros registros) {
    uint32_t resultado;
    int flagC = 0;
    int signo = ((int32_t)operandoA < 0);

    if (operandoB == 0) {
        resultado = operandoA;
    } else if (operandoB < 32) {
        resultado = operandoA >> operandoB;
        if (signo) {
            resultado |= ~(0xFFFFFFFFu >> operandoB);
        }
        flagC = (operandoA >> (operandoB - 1)) & 1u;
    } else {
        resultado = signo ? 0xFFFFFFFFu : 0;
        flagC = signo;
    }

    actualizarCC(registros, (int32_t)resultado < 0, resultado == 0, flagC, 0);
    return resultado;
}
