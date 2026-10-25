#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "consola.h"
#include "loader.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "errores.h"

int main(void) {
    printf("=== INICIO DE PRUEBAS UNITARIAS ETAPA 3 ===\n");

    // -------------------------------------------------------------
    // PRUEBA 1: Proceso solo con Code + Stack (Segmentos opcionales en cero)
    // -------------------------------------------------------------
    {
        printf("\n[TEST 1] Proceso solo con Code + Stack...\n");
        ConfiguracionMV cfg;
        inicializarConfiguracion(&cfg);
        cfg.archVMX = "tests/test_v2_code_stack.vmx";
        procesarHeader(cfg.archVMX, &cfg.header);

        assert(cfg.header.version == 2);
        assert(cfg.header.code_size == 16);
        assert(cfg.header.const_size == 0);
        assert(cfg.header.data_size == 0);
        assert(cfg.header.extra_size == 0);
        assert(cfg.header.stack_size == 512);

        tabla_segmentos t;
        cargarTablaSegmentosV2(t, &cfg);

        // Validar que solo Code (0) y Stack (1) estan en la tabla
        assert(obtenerBaseSegmento(t, 0) == 0);
        assert(obtenerTamanioSegmento(t, 0) == 16);
        assert(obtenerBaseSegmento(t, 1) == 16);
        assert(obtenerTamanioSegmento(t, 1) == 512);
        for (int i = 2; i < CANT_SEG; i++) {
            assert(t[i] == 0xFFFFFFFF);
        }

        // Validar inicializacion de registros (32 bits c/u: 16 altos num seg, 16 bajos offset)
        Registros r;
        inicializarRegistrosV2(r, t, &cfg);
        assert(r[REGPS] == 0xFFFFFFFF);
        assert(r[REGKS] == 0xFFFFFFFF);
        assert(r[REGCS] == 0x00000000); // Entrada 0, offset 0
        assert(r[REGDS] == 0xFFFFFFFF);
        assert(r[REGES] == 0xFFFFFFFF);
        assert(r[REGSS] == 0x00010000); // Entrada 1, offset 0
        assert(r[REGSP] == 0x00010200); // SS + 512
        assert(r[REGIP] == 0x00000000); // CS + entry point (0)

        // Validar reserva y carga en memoria
        Memoria m = NULL;
        uint32_t ramBytes = cfg.mem_kib * 1024;
        reservarMemoria(&m, ramBytes);
        cargarMemoriaV2(cfg.archVMX, m, ramBytes, t, r, &cfg.header);

        // Verificar que el codigo se cargo byte a byte correctamente
        uint32_t baseCode = obtenerBaseSegmento(t, r[REGCS] >> 16);
        for (int i = 0; i < 16; i += 4) {
            assert(m[baseCode + i + 0] == 0x90);
            assert(m[baseCode + i + 1] == 0x00);
            assert(m[baseCode + i + 2] == 0x00);
            assert(m[baseCode + i + 3] == 0x0A);
        }
        liberarMemoria(&m);
        printf(" -> PASO (OK)\n");
    }

    // -------------------------------------------------------------
    // PRUEBA 2: Proceso con todos los segmentos (Param, Const, Code, Data, Extra, Stack)
    // -------------------------------------------------------------
    {
        printf("\n[TEST 2] Proceso con todos los segmentos...\n");
        ConfiguracionMV cfg;
        inicializarConfiguracion(&cfg);
        cfg.archVMX = "tests/test_v2_all_segments.vmx";
        cfg.flagP = 1;
        cfg.cantParams = 2;
        char *dummyParams[] = {"param1", "param2"};
        cfg.params = dummyParams;
        procesarHeader(cfg.archVMX, &cfg.header);

        tabla_segmentos t;
        cargarTablaSegmentosV2(t, &cfg);

        // Validar que los 6 segmentos estan en la tabla y contiguos
        assert(obtenerBaseSegmento(t, 0) == 0); // Param
        assert(obtenerTamanioSegmento(t, 0) > 0);

        assert(obtenerBaseSegmento(t, 1) == obtenerBaseSegmento(t, 0) + obtenerTamanioSegmento(t, 0)); // Const
        assert(obtenerTamanioSegmento(t, 1) == 16);

        assert(obtenerBaseSegmento(t, 2) == obtenerBaseSegmento(t, 1) + obtenerTamanioSegmento(t, 1)); // Code
        assert(obtenerTamanioSegmento(t, 2) == 20);

        assert(obtenerBaseSegmento(t, 3) == obtenerBaseSegmento(t, 2) + obtenerTamanioSegmento(t, 2)); // Data
        assert(obtenerTamanioSegmento(t, 3) == 64);

        assert(obtenerBaseSegmento(t, 4) == obtenerBaseSegmento(t, 3) + obtenerTamanioSegmento(t, 3)); // Extra
        assert(obtenerTamanioSegmento(t, 4) == 32);

        assert(obtenerBaseSegmento(t, 5) == obtenerBaseSegmento(t, 4) + obtenerTamanioSegmento(t, 4)); // Stack
        assert(obtenerTamanioSegmento(t, 5) == 1024);

        assert(t[6] == 0xFFFFFFFF);
        assert(t[7] == 0xFFFFFFFF);

        // Validar registros
        Registros r;
        inicializarRegistrosV2(r, t, &cfg);
        assert(r[REGPS] == 0x00000000);
        assert(r[REGKS] == 0x00010000);
        assert(r[REGCS] == 0x00020000);
        assert(r[REGDS] == 0x00030000);
        assert(r[REGES] == 0x00040000);
        assert(r[REGSS] == 0x00050000);
        assert(r[REGSP] == 0x00050400); // Entrada 5, offset 1024
        assert(r[REGIP] == 0x00020004); // Entrada 2, entry point 4

        // Validar carga en RAM
        Memoria m = NULL;
        uint32_t ramBytes = cfg.mem_kib * 1024;
        reservarMemoria(&m, ramBytes);
        cargarParamSegment(&cfg, m, t, r);
        cargarMemoriaV2(cfg.archVMX, m, ramBytes, t, r, &cfg.header);

        // Constantes cargadas en Const Segment
        uint32_t baseConst = obtenerBaseSegmento(t, r[REGKS] >> 16);
        assert(memcmp(&m[baseConst], "CadenaConstante\0", 16) == 0);

        // Param Segment: primer string es el ejecutable
        uint32_t baseParam = obtenerBaseSegmento(t, r[REGPS] >> 16);
        assert(strcmp((char *)&m[baseParam], cfg.archVMX) == 0);

        liberarMemoria(&m);
        printf(" -> PASO (OK)\n");
    }

    // -------------------------------------------------------------
    // PRUEBA 3: Carga byte a byte exacta en test_v2.vmx
    // -------------------------------------------------------------
    {
        printf("\n[TEST 3] Carga byte a byte y metadatos de test_v2.vmx...\n");
        ConfiguracionMV cfg;
        inicializarConfiguracion(&cfg);
        cfg.archVMX = "tests/test_v2.vmx";
        procesarHeader(cfg.archVMX, &cfg.header);

        tabla_segmentos t;
        cargarTablaSegmentosV2(t, &cfg);

        Registros r;
        inicializarRegistrosV2(r, t, &cfg);

        Memoria m = NULL;
        uint32_t ramBytes = cfg.mem_kib * 1024;
        reservarMemoria(&m, ramBytes);
        cargarMemoriaV2(cfg.archVMX, m, ramBytes, t, r, &cfg.header);

        // Const Segment debe contener "Hola Mundo\0\0"
        uint32_t baseConst = obtenerBaseSegmento(t, r[REGKS] >> 16);
        assert(memcmp(&m[baseConst], "Hola Mundo\0\0", 12) == 0);

        // Code Segment debe coincidir byte a byte con el archivo
        uint32_t baseCode = obtenerBaseSegmento(t, r[REGCS] >> 16);
        uint8_t expectedCode[] = {0x90, 0x00, 0x00, 0x0A, 0x80, 0x00, 0x0F, 0x00, 0x00, 0x00};
        assert(memcmp(&m[baseCode], expectedCode, 10) == 0);

        liberarMemoria(&m);
        printf(" -> PASO (OK)\n");
    }

    // -------------------------------------------------------------
    // PRUEBA 4: Memoria RAM variable (m=M) y limites
    // -------------------------------------------------------------
    {
        printf("\n[TEST 4] Memoria RAM variable personalizada (m=32 KiB)...\n");
        ConfiguracionMV cfg;
        inicializarConfiguracion(&cfg);
        cfg.archVMX = "tests/test_v2.vmx";
        cfg.mem_kib = 32;
        procesarHeader(cfg.archVMX, &cfg.header);

        tabla_segmentos t;
        cargarTablaSegmentosV2(t, &cfg);

        Registros r;
        inicializarRegistrosV2(r, t, &cfg);

        Memoria m = NULL;
        reservarMemoria(&m, cfg.mem_kib * 1024);
        assert(m != NULL);
        cargarMemoriaV2(cfg.archVMX, m, cfg.mem_kib * 1024, t, r, &cfg.header);
        liberarMemoria(&m);
        assert(m == NULL);
        printf(" -> PASO (OK)\n");
    }

    printf("\n=== TODAS LAS PRUEBAS UNITARIAS DE LA ETAPA 3 PASARON EXITOSAMENTE ===\n");
    return 0;
}
