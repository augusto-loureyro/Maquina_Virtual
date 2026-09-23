#ifndef MAQUINAVIRTUAL_H_INCLUDED
#define MAQUINAVIRTUAL_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define DIMMEMORIA 16384 /// 16 KiB = 16384 bytes
#define REGISTROS 32
#define SEGMENTOS 8

/// Identificadores de registros
/// Instrucciones
#define REGIP  0
#define REGOPC 1
#define REGOP1 2
#define REGOP2 3
/// Accesos a memoria
#define REGLAR 4
#define REGMAR 5
#define REGMBR 6
//#define REG 7
//#define REG 8
//#define REG 9
/// Registros de propositos generales
#define REGEAX 10
#define REGEBX 11
#define REGECX 12
#define REGEDX 13
#define REGEEX 14
#define REGEFX 15
/// Acumulador
#define REGAC  16
/// Codigo de condicion
#define REGCC  17
//#define REG 18
//#define REG 19
//#define REG 20
//#define REG 21
//#define REG 22
//#define REG 23
//#define REG 24
//#define REG 25
/// Segmenos
#define REGCS  26
#define REGDS  27
//#define REG 28
//#define REG 29
//#define REG 30
//#define REG 31

/// Constantes para el registro CC
#define NBIT 31
#define ZBIT 30
#define CBIT 29
#define OBIT 28

#define NMASK (1 << NBIT)  // 2147483648
#define ZMASK (1 << ZBIT)  // 1073741824
#define CMASK (1 << CBIT)  // 536870912
#define OMASK (1 << OBIT)  // 268435456

typedef struct {
    int8_t memoria[DIMMEMORIA]; /// Direcciones fisicas
    uint32_t segTabla[SEGMENTOS]; /// 2 bytes base (dir fisica), 2 bytes tamanio en bytes
    int32_t registros[REGISTROS];
    bool running, disassembler;
}ETMaquinaVirtual;


void mvInic(ETMaquinaVirtual *maqVirt, bool disassembler);
void cargarArchivo(ETMaquinaVirtual *maqVirt, char *nombreArchivo, unsigned int *tamanioCode);
void mvEjecutar(ETMaquinaVirtual *maqVirt, unsigned int tamanioCode);
void setFlags(ETMaquinaVirtual *maqVirt, bool n, bool z, bool c, bool o);
void mvError(ETMaquinaVirtual *maqVirt, char *mensaje);
void guardarResult(ETMaquinaVirtual *maqVirt, int tipoOp, int valorOp, int result);


#endif // MAQUINAVIRTUAL_H_INCLUDED
