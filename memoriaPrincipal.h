#ifndef MEMORIAPRINCIPAL_H_INCLUDED
#define MEMORIAPRINCIPAL_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>

/// Traducción de dirección lógica a física. Retorna -1 si hay error (Fallo de segmento).
int cambioLogicFisic(ETMaquinaVirtual *maqVirt, int dirLogica);
/// Fetch de instrucciones (lee un byte desde la memoria física y avanza el puntero, sin afectar LAR/MAR/MBR)
char siguienteInstruc(ETMaquinaVirtual *maqVirt, int *dirLogica);


#endif // MEMORIAPRINCIPAL_H_INCLUDED
