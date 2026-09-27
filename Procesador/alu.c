//se debe encargar de realizar operaciones aritméticas y lógicas en 32 bits 
#include "alu.h"
#include"errores.h"

void actualizarCC(Registros r, int n, int z, int c, int v){
    uint32_t cc = 0;

    if (n) cc |= (1u << CC_BIT_N);
    if (z) cc |= (1u << CC_BIT_Z);
    if (c) cc |= (1u << CC_BIT_C);
    if (v) cc |= (1u << CC_BIT_V);

    r[REGCC] = cc;
}

uint32_t sumar(uint32_t a, uint32_t b, Registros r){
    uint64_t sumaCompleta = (uint64_t)a + (uint64_t)b;
    uint32_t resultado = (uint32_t)sumaCompleta;

    int signoA = (int32_t)a < 0;
    int signoB = (int32_t)b < 0;
    int signoResultado = (int32_t)resultado < 0;

    int n = signoResultado;
    int z = (resultado == 0);
    int c = (sumaCompleta > 0xFFFFFFFFu);
    int v = (signoA == signoB) && (signoResultado != signoA);

    actualizarCC(r, n, z, c, v);

    return resultado;
}

uint32_t restar(uint32_t a, uint32_t b, Registros r){
    uint64_t restaCompleta = (uint64_t)a + (uint64_t)(~b) + 1ULL; 
    uint32_t resultado = (uint32_t)restaCompleta;

    int signoA = (int32_t)a < 0;
    int signoB = (int32_t)b < 0;
    int signoResultado = (int32_t)resultado < 0;

    int n = signoResultado;
    int z = (resultado == 0);
    int c = (restaCompleta > 0xFFFFFFFFu);
    int v = (signoA != signoB) && (signoResultado != signoA);

    actualizarCC(r, n, z, c, v);

    return resultado;
}

uint32_t multiplicar(uint32_t a, uint32_t b, Registros r){
    uint64_t productoSinSigno = (uint64_t)a * (uint64_t)b;
    int64_t productoConSigno = (int64_t)(int32_t)a * (int64_t)(int32_t)b;
    uint32_t resultado = (uint32_t)productoSinSigno;

    int n = (int32_t)resultado < 0;
    int z = (resultado == 0);
    int c = (productoSinSigno > 0xFFFFFFFFu);
    int v = (productoConSigno < INT32_MIN) || (productoConSigno > INT32_MAX); // !! revisar toda la lógica de modificación del CC


    actualizarCC(r, n, z, c, v);

    return resultado;
}

uint32_t dividir(uint32_t a, uint32_t b, Registros r){
    if (b == 0){
        reportarError(ERROR_DIVISION_POR_CERO);
    }

    int overflow = ((int32_t)a == INT32_MIN) && ((int32_t)b == -1); //único caso puntual: a / b = -2147483648 / -1 = 2147483648 -> 32bit = [-2147483648,-2147483647] 

    int32_t cociente, resto;
    if (overflow) {
        cociente = INT32_MIN; // el resultado real no entra en 32 bits con signo
        resto = 0;
    } else {
        cociente = (int32_t)a / (int32_t)b;
        resto = (int32_t)a % (int32_t)b;
    }

    r[REGAC] = (uint32_t)resto;

    int n = (cociente < 0);
    int z = (cociente == 0);
    int c = 0;       
    int v = overflow;

    actualizarCC(r, n, z, c, v);
    return (uint32_t)cociente;
}

uint32_t and_logico(uint32_t a, uint32_t b, Registros r){
    uint32_t resultado = a&b;

    int signoResultado = (int32_t)resultado < 0;

    int n = signoResultado;
    int z = (resultado == 0);
    int c = 0;
    int v = 0;

    actualizarCC(r, n, z, c, v);

    return resultado;
}
uint32_t or_logico(uint32_t a, uint32_t b, Registros r){
    uint32_t resultado = a|b;

    int signoResultado = (int32_t)resultado < 0;

    int n = signoResultado;
    int z = (resultado == 0);
    int c = 0;
    int v = 0;

    actualizarCC(r, n, z, c, v);

    return resultado;
}
uint32_t xor_logico(uint32_t a, uint32_t b, Registros r){
    uint32_t resultado = a^b;

    int signoResultado = (int32_t)resultado < 0;

    int n = signoResultado;
    int z = (resultado == 0);
    int c = 0;
    int v = 0;

    actualizarCC(r, n, z, c, v);

    return resultado;
}
uint32_t not_logico(uint32_t a, Registros r){
    uint32_t resultado = ~a;

    int signoResultado = (int32_t)resultado < 0;

    int n = signoResultado;
    int z = (resultado == 0);
    int c = 0;
    int v = 0;

    actualizarCC(r, n, z, c, v);

    return resultado;
}

//---

uint32_t shift_izq(uint32_t a, uint32_t b, Registros r){
    uint32_t resultado;
    int c = 0, v = 0;

    if (b == 0) {
        resultado = a;  
    } else if (b < 32) {
        resultado = a << b;
        //c = (a >> (32 - b)) & 1u;           // último bit que "salió" por la izquierda

        uint32_t bitsQueSalen = a & (0xFFFFFFFFu << (32 - b)); // los b bits más altos de a
        c = (bitsQueSalen != 0);

        // valor exacto (sin truncar) vs. valor truncado a 32 bits
        int64_t exacto = (int64_t)(int32_t)a * (1LL << b);
        v = (exacto != (int64_t)(int32_t)resultado);
    } else {
        resultado = 0;      // en C, a << 32 o más saría comportamiento indefinido
        c = (b == 32) ? (a & 1u) : 0;
        v = (a != 0);
    }

    int n = ((int32_t)resultado < 0);
    int z = (resultado == 0);

    actualizarCC(r, n, z, c, v);
    return resultado;
}
uint32_t shift_der(uint32_t a, uint32_t b, Registros r){
    uint32_t resultado;
    int c = 0, v = 0;

    if (b == 0) {
        resultado = a;                      // sin desplazamiento: C=0
    } else if (b < 32) {
        resultado = a >> b;
        c = (a >> (b - 1)) & 1u;            // último bit que salió por la derecha
    } else {
        resultado = 0;
        c = (b == 32) ? ((a >> 31) & 1u) : 0;
    }

    int n = ((int32_t)resultado < 0);
    int z = (resultado == 0);

    actualizarCC(r, n, z, c, v);
    return resultado;
}
uint32_t shift_der_aritmetico(uint32_t a, uint32_t b, Registros r){
    uint32_t resultado;
    int c = 0, v = 0;
    int signo = ((int32_t)a < 0);

    if (b == 0) {
        resultado = a;
    } else if (b < 32) {
        // rellenar con el signo a mano, para no depender de que el
        // compilador haga >> aritmético sobre negativos (es implementation-defined)
        resultado = a >> b;
        if (signo)
            resultado |= ~(0xFFFFFFFFu >> b);
        c = (a >> (b - 1)) & 1u;
    } else {
        resultado = signo ? 0xFFFFFFFFu : 0;   // todos los bits son copias del signo
        c = signo;                              // lo último que sale es una copia del signo
    }

    int n = ((int32_t)resultado < 0);
    int z = (resultado == 0);

    actualizarCC(r, n, z, c, v);
    return resultado;
}

void actualizarFlagsValor(uint32_t valor, Registros r){
    int n = (int32_t)valor < 0;
    int z = (valor == 0);

    actualizarCC(r, n, z, 0, 0);
}