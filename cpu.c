#include "cpu.h"
//------
// Arma el valor de OP1/OP2: byte alto = tipo, los 3 bytes restantes = el
// valor tal cual está codificado en memoria (0 a 3 bytes, alineado a la
// derecha, con ceros a la izquierda si el operando ocupa menos de 3 bytes).
uint32_t armarRegistroOperando(int tipo, uint8_t *bytes){
    uint32_t valor = 0;
    int tam = tamanioOperando(tipo);
 
    for (int i = 0; i < tam; i++){
        valor = (valor << 8) | bytes[i];
    }
 
    return ((uint32_t)tipo << 24) | valor;
}
Instruccion buscarInstruccion(tabla_segmentos t, Memoria m, Registros r){
    Instruccion instr;
 
    dirFisica dirInstr = traducir(r[REGIP], t, r, 1, 0); // fetch: no es acceso a memoria
    uint8_t primerByte = m[dirInstr];
 
    decodificarPrimerByte(primerByte, &instr.opcode, &instr.categoria);
 
    int tipoA, tipoB;
    extraerTiposOperando(primerByte, instr.categoria, &tipoA, &tipoB);
 
    int tamB = tamanioOperando(tipoB);
    int tamA = tamanioOperando(tipoA);
 
    uint8_t *bytesB = &m[dirInstr + 1];
    uint8_t *bytesA = &m[dirInstr + 1 + tamB];
 
    instr.operandoB = leerOperando(tipoB, bytesB);
    instr.operandoA = leerOperando(tipoA, bytesA);
 
    r[REGOPC] = instr.opcode;
    r[REGOP1] = armarRegistroOperando(tipoA, bytesA);
    r[REGOP2] = armarRegistroOperando(tipoB, bytesB);
 
    int largoTotal = 1 + tamB + tamA;
    uint16_t nuevoOffset = (uint16_t)(r[REGIP] & 0xFFFF) + largoTotal;
    r[REGIP] = (r[REGIP] & 0xFFFF0000) | nuevoOffset;
 
    return instr;
}
uint32_t leerValorOperando(Operando o, tabla_segmentos t, Memoria m, Registros r){
    dirLogica l;
    dirFisica f;
 
    switch (o.tipo){
        case TIPO_REGISTRO:
            return r[o.codigoRegistro];
        case TIPO_INMEDIATO:
            return (uint32_t)(int32_t)o.inmediato;
        case TIPO_MEMORIA:
            l = r[o.codigoRegistro] + o.desplazamiento;
            f = traducir(l, t, r, TAMANIO_DATO, 1); // 1: es acceso a memoria (carga LAR/MAR)
            return leerDeMemoria(m, f, r);           // carga MBR
        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            return 0;
    }
}
 
void escribirValorOperando(Operando o, uint32_t valor, tabla_segmentos t, Memoria m, Registros r){
    dirLogica l;
    dirFisica f;
 
    switch (o.tipo){
        case TIPO_REGISTRO:
            r[o.codigoRegistro] = valor;
            break;
        case TIPO_MEMORIA:
            l = r[o.codigoRegistro] + o.desplazamiento;
            f = traducir(l, t, r, TAMANIO_DATO, 1);
            escribirEnMemoria(m, f, valor, r);
            break;
        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
    }
}
#define OPCODE_STOP 0x0F
#define OPCODE_MOV  0x10
 
// ¿Sigue habiendo una instrucción por ejecutar? Es la condición del loop:
// se repite mientras IP apunte dentro del segmento de código. No pasa por
// traducir() a propósito, para no reportar error cuando el programa
// simplemente termina (con o sin STOP).
int hayMasInstrucciones(tabla_segmentos t, Registros r){
    int segmento = r[REGIP] >> 16;
    uint16_t offset = r[REGIP] & 0xFFFF;
 
    return (segmento == CODE) && (offset < obtenerTamanioSegmento(t, CODE));
}
 
void ejecutarInstruccion(Instruccion instr, tabla_segmentos t, Memoria m, Registros r){
    uint32_t valor;
 
    switch (instr.opcode){
        case OPCODE_STOP:
            r[REGIP] = 0xFFFFFFFF;
            break;
        case OPCODE_MOV:
            valor = leerValorOperando(instr.operandoB, t, m, r);
            escribirValorOperando(instr.operandoA, valor, t, m, r);
            break;
        default:
            // El resto de las instrucciones todavía no están implementadas.
            reportarError(ERROR_INSTRUCCION_INVALIDA);
    }
}
 
void ejecutarPrograma(tabla_segmentos t, Memoria m, Registros r){
    while (hayMasInstrucciones(t, r)){
        Instruccion instr = buscarInstruccion(t, m, r);
        ejecutarInstruccion(instr, t, m, r);
    }
}