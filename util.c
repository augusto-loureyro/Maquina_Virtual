#include "include/util.h"

uint32_t LCH(uint32_t b, uint16_t h){
    b &= 0x0000FFFF;        // borro la parte alta
    b |= ((uint32_t)h << 16); // pongo la nueva parte alta
    return b;
}
uint32_t LCL(uint32_t b, uint16_t l){
    b &= 0xFFFF0000;   // borro la parte baja
    b |= l;            // cargo la nueva parte baja
    return b;
}
