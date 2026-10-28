#include <stdio.h>
#include "registros.h"
#include "tablaSegmentos.h"

void inicializarRegistrosV1(Registros registros) {
    for (int i = 0; i < CANT_REGISTROS; i++) {
        registros[i] = 0;
    }

    /// CS y DS: 16 bits altos = numero de segmento en la tabla, 16 bits bajos = 0
    registros[REGCS] = (uint32_t)CODE << 16;
    registros[REGDS] = (uint32_t)DATA << 16;

    /// IP apunta a la primera instruccion dentro del segmento de codigo (mismo valor inicial que CS)
    registros[REGIP] = registros[REGCS];
}

void inicializarRegistrosV2(Registros registros, const uint32_t tablaSegmentos[], const ConfiguracionMV *cfg) {
    (void)tablaSegmentos;
    for (int i = 0; i < CANT_REGISTROS; i++) {
        registros[i] = 0;
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
        registros[REGPS] = (uint32_t)idx++ << 16;
    } else {
        registros[REGPS] = 0xFFFFFFFF;
    }

    /// KS (Const Segment)
    if (const_size > 0) {
        registros[REGKS] = (uint32_t)idx++ << 16;
    } else {
        registros[REGKS] = 0xFFFFFFFF;
    }

    /// CS (Code Segment)
    if (code_size > 0) {
        registros[REGCS] = (uint32_t)idx++ << 16;
    } else {
        registros[REGCS] = 0xFFFFFFFF;
    }

    /// DS (Data Segment)
    if (data_size > 0) {
        registros[REGDS] = (uint32_t)idx++ << 16;
    } else {
        registros[REGDS] = 0xFFFFFFFF;
    }

    /// ES (Extra Segment)
    if (extra_size > 0) {
        registros[REGES] = (uint32_t)idx++ << 16;
    } else {
        registros[REGES] = 0xFFFFFFFF;
    }

    /// SS (Stack Segment)
    if (stack_size > 0) {
        registros[REGSS] = (uint32_t)idx++ << 16;
    } else {
        registros[REGSS] = 0xFFFFFFFF;
    }

    /// SP: 16 bits mas significativos = numero de segmento de SS, 16 bits menos significativos = stack_size
    if (registros[REGSS] != 0xFFFFFFFF) {
        registros[REGSP] = registros[REGSS] + stack_size;
    } else {
        registros[REGSP] = 0xFFFFFFFF;
    }

    /// IP: 16 bits mas significativos = numero de segmento de CS, 16 bits menos significativos = entry_point
    if (registros[REGCS] != 0xFFFFFFFF) {
        registros[REGIP] = registros[REGCS] | (cfg->header.entry_point & 0xFFFF);
    } else {
        registros[REGIP] = 0xFFFFFFFF;
    }
}

void mostrarRegistros(const Registros registros) {
    printf("Registros de Segmentos y Control (Version 2):\n");
    printf("  PS (Param Segment): ");
    if (registros[REGPS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", registros[REGPS], registros[REGPS] >> 16, registros[REGPS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  KS (Const Segment): ");
    if (registros[REGKS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", registros[REGKS], registros[REGKS] >> 16, registros[REGKS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  CS (Code Segment) : ");
    if (registros[REGCS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", registros[REGCS], registros[REGCS] >> 16, registros[REGCS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  DS (Data Segment) : ");
    if (registros[REGDS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", registros[REGDS], registros[REGDS] >> 16, registros[REGDS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  ES (Extra Segment): ");
    if (registros[REGES] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", registros[REGES], registros[REGES] >> 16, registros[REGES] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  SS (Stack Segment): ");
    if (registros[REGSS] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", registros[REGSS], registros[REGSS] >> 16, registros[REGSS] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  SP (Stack Pointer): ");
    if (registros[REGSP] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Offset 0x%04X)\n", registros[REGSP], registros[REGSP] >> 16, registros[REGSP] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");

    printf("  IP (Instr Pointer): ");
    if (registros[REGIP] != 0xFFFFFFFF) printf("0x%08X (Entrada %u, Entry Offset 0x%04X)\n", registros[REGIP], registros[REGIP] >> 16, registros[REGIP] & 0xFFFF);
    else printf("0xFFFFFFFF (-1 / no asignado)\n");
}

//venían en big-endian?? asumo que sí
void cargarRegistrosVMI(const uint8_t *bytes, Registros registros) {
    for (int i = 0; i < CANT_REGISTROS; i++) {
        registros[i] = bytesAUint32BE(&bytes[i*4]);
    }
}