#ifndef REGISTROS_H
#define REGISTROS_H

#include <stdint.h>
#include "tablaSegmentos.h"
#include "util.h"


#define CANT_REGISTROS 32
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


typedef uint32_t Registros[CANT_REGISTROS];

void inicializarRegistros(Registros r);

#endif