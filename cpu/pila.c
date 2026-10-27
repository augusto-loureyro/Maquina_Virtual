#include "pila.h"
#include "errores.h"
#include "traductor.h"

void push(uint32_t valor, tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros) {
    int segSS = (int)(registros[REGSS] >> 16);

    /// Si no existe el segmento de stack, no hay pila utilizable
    if (registros[REGSS] == 0xFFFFFFFF || !esSegmentoValido(tablaSegmentos, segSS)) {
        reportarError(ERROR_STACK_OVERFLOW);
    }

    /// Validar que SP pertenezca al segmento SS
    if ((int)(registros[REGSP] >> 16) != segSS) {
        reportarError(ERROR_STACK_OVERFLOW);
    }

    uint16_t offsetSP = (uint16_t)(registros[REGSP] & 0xFFFF);

    /// Paso 1 y 2: Si el valor de SP tras decrementar en 4 es menor que SS, es STACK OVERFLOW
    if (offsetSP < 4 || (registros[REGSP] - 4) < registros[REGSS]) {
        reportarError(ERROR_STACK_OVERFLOW);
    }

    registros[REGSP] -= 4;

    /// Paso 5: Almacenar en la pila desde los bytes menos significativos, dejando en el tope el byte mas significativo (big-endian)
    dirFisica dirFisicaDestino = traducir(registros[REGSP], tablaSegmentos, registros, 4, 1);
    escribirEnMemoria(memoria, dirFisicaDestino, valor, 4, registros);
}


uint32_t pop(tabla_segmentos tablaSegmentos, Memoria memoria, Registros registros) {
    int segSS = (int)(registros[REGSS] >> 16);

    /// Si no existe el segmento de stack, no se puede extraer
    if (registros[REGSS] == 0xFFFFFFFF || !esSegmentoValido(tablaSegmentos, segSS)) {
        reportarError(ERROR_STACK_UNDERFLOW);
    }

    /// Validar que SP pertenezca al segmento SS
    if ((int)(registros[REGSP] >> 16) != segSS) {
        reportarError(ERROR_STACK_UNDERFLOW);
    }

    uint32_t tamSS = (uint32_t)obtenerTamanioSegmento(tablaSegmentos, segSS);
    uint32_t offsetSP = (uint32_t)(registros[REGSP] & 0xFFFF);

    /// Paso 2: Si no habia bytes suficientes (menos de 4) o la pila estaba vacia, es STACK UNDERFLOW
    if (registros[REGSP] < registros[REGSS] || offsetSP + 4 > tamSS) {
        reportarError(ERROR_STACK_UNDERFLOW);
    }

    /// Paso 1 y 3: Extraer 4 bytes desde el tope de la pila en orden big-endian
    dirFisica dirFisicaOrigen = traducir(registros[REGSP], tablaSegmentos, registros, 4, 1);
    uint32_t valor = leerDeMemoria(memoria, dirFisicaOrigen, 4, registros);

    /// Paso 5: Incrementar el valor del SP en 4
    registros[REGSP] += 4;

    return valor;
}

