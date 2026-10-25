#include <stdio.h>
#include "registros.h"
#include "tablaSegmentos.h"

void inicializarRegistrosV1(Registros r) {
    for (int i = 0; i < CANT_REGISTROS; i++) {
        r[i] = 0;
    }

    /// CS y DS: 16 bits altos = numero de segmento en la tabla, 16 bits bajos = 0
    r[REGCS] = (uint32_t)CODE << 16;
    r[REGDS] = (uint32_t)DATA << 16;

    /// IP apunta a la primera instruccion dentro del segmento de codigo (mismo valor inicial que CS)
    r[REGIP] = r[REGCS];
}

void inicializarRegistrosV2(Registros r, const uint32_t t[], const ConfiguracionMV *cfg) {
    (void)t;
    for (int i = 0; i < CANT_REGISTROS; i++) {
        r[i] = 0;
    }

    uint16_t param_size = calcularTamanioParamSegment(cfg);
    uint16_t const_size = cfg->header.const_size;
    uint16_t code_size  = cfg->header.code_size;
    uint16_t data_size  = cfg->header.data_size;
    uint16_t extra_size = cfg->header.extra_size;
    uint16_t stack_size = cfg->header.stack_size;

    int idx = 0;

    /// Registros de segmentos (32 bits):
    /// 16 bits mas significativos = numero de segmento en la tabla
    /// 16 bits menos significativos = desplazamiento inicial (0x0000)
    /// Si no existe, se carga con -1 (0xFFFFFFFF)

    /// PS (Param Segment)
    if (param_size > 0) {
        r[REGPS] = (uint32_t)idx++ << 16;
    } else {
        r[REGPS] = 0xFFFFFFFF;
    }

    /// KS (Const Segment)
    if (const_size > 0) {
        r[REGKS] = (uint32_t)idx++ << 16;
    } else {
        r[REGKS] = 0xFFFFFFFF;
    }

    /// CS (Code Segment)
    if (code_size > 0) {
        r[REGCS] = (uint32_t)idx++ << 16;
    } else {
        r[REGCS] = 0xFFFFFFFF;
    }

    /// DS (Data Segment)
    if (data_size > 0) {
        r[REGDS] = (uint32_t)idx++ << 16;
    } else {
        r[REGDS] = 0xFFFFFFFF;
    }

    /// ES (Extra Segment)
    if (extra_size > 0) {
        r[REGES] = (uint32_t)idx++ << 16;
    } else {
        r[REGES] = 0xFFFFFFFF;
    }

    /// SS (Stack Segment)
    if (stack_size > 0) {
        r[REGSS] = (uint32_t)idx++ << 16;
    } else {
        r[REGSS] = 0xFFFFFFFF;
    }

    /// SP: 16 bits mas significativos = numero de segmento de SS, 16 bits menos significativos = stack_size
    if (r[REGSS] != 0xFFFFFFFF) {
        r[REGSP] = r[REGSS] + stack_size;
    } else {
        r[REGSP] = 0xFFFFFFFF;
    }

    /// IP: 16 bits mas significativos = numero de segmento de CS, 16 bits menos significativos = entry_point
    if (r[REGCS] != 0xFFFFFFFF) {
        r[REGIP] = r[REGCS] | (cfg->header.entry_point & 0xFFFF);
    } else {
        r[REGIP] = 0xFFFFFFFF;
    }
}

void mostrarRegistros(const Registros r) {
    printf("Registros de Segmentos y Control (Version 2):\n");
    printf("  PS (Param Segment): ");
    if (r[REGPS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", r[REGPS], r[REGPS] >> 16, r[REGPS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  KS (Const Segment): ");
    if (r[REGKS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", r[REGKS], r[REGKS] >> 16, r[REGKS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  CS (Code Segment) : ");
    if (r[REGCS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", r[REGCS], r[REGCS] >> 16, r[REGCS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  DS (Data Segment) : ");
    if (r[REGDS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", r[REGDS], r[REGDS] >> 16, r[REGDS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  ES (Extra Segment): ");
    if (r[REGES] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", r[REGES], r[REGES] >> 16, r[REGES] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  SS (Stack Segment): ");
    if (r[REGSS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", r[REGSS], r[REGSS] >> 16, r[REGSS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  SP (Stack Pointer): ");
    if (r[REGSP] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", r[REGSP], r[REGSP] >> 16, r[REGSP] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  IP (Instr Pointer): ");
    if (r[REGIP] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Entry Offset 0x%04X)\n", r[REGIP], r[REGIP] >> 16, r[REGIP] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");
}
