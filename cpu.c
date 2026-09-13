#include "cpu.h"
#include "traductor.h"
#include "errores.h"

Instruccion buscarInstruccion(tabla_segmentos t, Memoria m, Registros r){ //hecha completamente con IA, me perdí
    Instruccion instr;
    int categoria, tipoA, tipoB;

    dirFisica dirInstr = traducir(r[REGIP], t, 1, 0); // 0: sin verificar límite (fetch)
    uint8_t primerByte = m[dirInstr];

    decodificarPrimerByte(primerByte, &instr.opcode, &categoria);
    extraerTiposOperando(primerByte, categoria, &tipoA, &tipoB);

    int tamB = tamanioOperando(tipoB);
    int tamA = tamanioOperando(tipoA);

    instr.operandoB = leerOperando(tipoB, &m[dirInstr + 1]);
    instr.operandoA = leerOperando(tipoA, &m[dirInstr + 1 + tamB]);

    int largoTotal = 1 + tamB + tamA;
    uint16_t nuevoOffset = (r[REGIP] & 0xFFFF) + largoTotal;
    r[REGIP] = (r[REGIP] & 0xFFFF0000) | nuevoOffset;

    return instr;
}

dirFisica calcularDireccionMemoria(Operando o, tabla_segmentos t, Registros r){
    dirLogica l = r[o.codigoRegistro] + o.desplazamiento;
    dirFisica f = traducir(l, t, 4, 1); // 4 bytes, con verificación de límite

    r[REGLAR] = l;
    r[REGMAR] = (4 << 16) | (f & 0xFFFF);

    return f;
}

uint32_t leerMemoria4Bytes(Memoria m, dirFisica f){
    return (m[f] << 24) | (m[f+1] << 16) | (m[f+2] << 8) | m[f+3];
}

void escribirMemoria4Bytes(Memoria m, dirFisica f, uint32_t valor){
    m[f]   = (valor >> 24) & 0xFF;
    m[f+1] = (valor >> 16) & 0xFF;
    m[f+2] = (valor >> 8) & 0xFF;
    m[f+3] = valor & 0xFF;
}

uint32_t leerValorOperando(Operando o, tabla_segmentos t, Memoria m, Registros r){
    dirFisica f;
    uint32_t valor;

    switch (o.tipo){
        case TIPO_REGISTRO:
            return r[o.codigoRegistro];
        case TIPO_INMEDIATO:
            return (uint32_t)(int32_t)o.inmediato; // extiende el signo a 32 bits
        case TIPO_MEMORIA:
            f = calcularDireccionMemoria(o, t, r);
            valor = leerMemoria4Bytes(m, f);
            r[REGMBR] = valor;
            return valor;
        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
            return 0;
    }
}

void escribirValorOperando(Operando o, uint32_t valor, tabla_segmentos t, Memoria m, Registros r){
    dirFisica f;

    switch (o.tipo){
        case TIPO_REGISTRO:
            r[o.codigoRegistro] = valor;
            break;
        case TIPO_MEMORIA:
            f = calcularDireccionMemoria(o, t, r);
            escribirMemoria4Bytes(m, f, valor);
            r[REGMBR] = valor;
            break;
        default:
            reportarError(ERROR_INSTRUCCION_INVALIDA);
    }
}

// IA pura y dura:

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
        r[REGOPC] = instr.opcode;
        ejecutarInstruccion(instr, t, m, r);
    }
}