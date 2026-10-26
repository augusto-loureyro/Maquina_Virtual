#include <stdio.h>
#include <stdlib.h>

#include "include/consola.h"
#include "include/loader.h"
#include "include/tablaSegmentos.h"
#include "include/memoria.h"
#include "include/registros.h"
#include "include/cpu.h"

static void imprimirEncabezado(const HeaderVMX *hdr) {
    printf("Version: %d\n", hdr->version);
    printf("tamanio del seg. de codigo: %u\n", hdr->code_size);
    printf("tamanio del seg. de datos: %u\n", hdr->data_size);
    printf("tamanio del seg. extra: %u\n", hdr->extra_size);
    printf("tamanio del seg. del stack: %u\n", hdr->stack_size);
    printf("tamanio del seg. de constantes: %u\n", hdr->const_size);
    printf("offset del ep: %u\n", hdr->entry_point);
}

static void imprimirTablaSegmentos(tabla_segmentos tabla) {
    printf("\n----------------------------------------");
    printf("\n num segmento \t base \t tamanio ");
    for (int i = 0; i < CANT_SEG; i++) {
        printf("\n----------------------------------------");
        printf("\n\t %d \t %X \t %X ", i,
               obtenerBaseSegmento(tabla, i),
               obtenerTamanioSegmento(tabla, i));
    }
}

static void imprimirRegistros(const char *titulo, Registros registros) {
    printf("\n---------------------------------------------\n");
    printf("%s\n", titulo);
    for (int i = 0; i < CANT_REGISTROS; i++) {
        printf("%d \t\t %08X\n", i, registros[i]);
    }
}

int main(int argc, char *argv[]) {
    ConfiguracionMV config;
    tabla_segmentos tabla;
    IndiceSgm indices;
    Memoria memoria = NULL;
    Registros registros;
    uint32_t memoriaBytes;
    uint32_t bytesSegmentos;

    inicializarConfiguracion(&config);
    parsearArgumentos(argc, argv, &config);

    if (config.archVMX == NULL) {
        mostrarConfiguracion(&config);
        return 0;
    }

    procesarHeader(config.archVMX, &config.header);
    if (config.header.version == 1) {
        config.mem_kib = MEMORIA_POR_DEFECTO_KIB;
        if (config.header.code_size > TAMANIO_MEMORIA_DEFAULT) {
            reportarError(ERROR_MEMORIA_INSUFICIENTE);
        }
    }

    memoriaBytes = config.mem_kib * BYTES_POR_KIB;
    bytesSegmentos = (uint32_t)config.header.param_size +
                     config.header.const_size +
                     config.header.code_size +
                     config.header.data_size +
                     config.header.extra_size +
                     config.header.stack_size;
    if (bytesSegmentos > memoriaBytes || bytesSegmentos > UINT16_MAX + 1u) {
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }

    mostrarConfiguracion(&config);

    reservarMemoria(&memoria, config.mem_kib);
    cargarMemoria(config.archVMX, memoria, &config);
    imprimirEncabezado(&config.header);
    cargarTablaSegmentos(tabla, &indices, config.header);

    imprimirTablaSegmentos(tabla);
    inicializarRegistros(registros, indices);
    registros[REGIP] = ((uint32_t)indices.cs << 16) | config.header.entry_point;
    registros[REGSP] = ((uint32_t)indices.ss << 16) | config.header.stack_size;
    imprimirRegistros("Registros iniciales:", registros);

    printf("\nEjecutando programa...\n");
    ejecutarPrograma(tabla, memoria, registros, config.flagD, indices);

    imprimirRegistros("Registros al finalizar:", registros);
    free(memoria);
    return 0;
}