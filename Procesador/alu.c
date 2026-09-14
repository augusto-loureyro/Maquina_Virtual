//se debe encargar de realizar operaciones aritméticas y lógicas en 32 bits 
#include "../include/alu.h"

static void actualizarCC(Registros r, int n, int z, int c, int v){
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

void actualizarFlagsValor(uint32_t valor, Registros r){
    int n = (int32_t)valor < 0;
    int z = (valor == 0);

    actualizarCC(r, n, z, 0, 0);
}