#include <stdio.h>

#include "include/consola.h"
#include "include/loader.h"
#include "include/tablaSegmentos.h"
#include "include/memoria.h"
#include "include/registros.h"
#include "include/cpu.h"

static void imprimirConfiguracion(const Config *config) {
    printf("Archivo vmx: %s\n", config->archivoVMX);
    printf("Archivo vmi: %s\n", config->archivoVMI);
    printf("Memoria: %s\n", config->flagM ? "si" : "no");
    if (config->flagM) {
        printf("tamanio solicitado: %d\n", config->sizeM);
    }
    printf("Desensamblar: %s\n", config->flagD ? "si" : "no");
    printf("Parametros p: %s\n", config->flagP ? "si" : "no");

    for (int i = 0; i < config->nP; i++) {
        printf("\t p_%02d : %s\n", i, config->p[i]);
    }
}

static void imprimirEncabezado(const Header *hdr) {
    printf("Version: %d\n", hdr->version);
    printf("tamanio del seg. de codigo: %X\n", hdr->sizeCS);
    printf("tamanio del seg. de datos: %X\n", hdr->sizeDS);
    printf("tamanio del seg. extra: %X\n", hdr->sizeES);
    printf("tamanio del seg. del stack: %X\n", hdr->sizeSS);
    printf("tamanio del seg. de constantes: %X\n", hdr->sizeKS);
    printf("offset del ep: %X\n", hdr->offsetEP);
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
    Config config;
    Header hdr;
    tabla_segmentos tabla;
    IndiceSgm indices;
    Memoria memoria;
    Registros registros;

    parsearArgumentos(argc, argv, &config);
    imprimirConfiguracion(&config);

    procesarHeaderVMX(config.archivoVMX, &hdr);
    if (!config.flagM || hdr.version == 1) {
        config.sizeM = TAMANIO_MEMORIA_DEFAULT;
    }
    imprimirEncabezado(&hdr);

    if (hdr.sizeCS + hdr.sizeDS + hdr.sizeES + hdr.sizeKS +
        hdr.sizeSS + hdr.sizePS > config.sizeM) {
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }

    cargarTablaSegmentos(tabla, &indices, hdr);
    imprimirTablaSegmentos(tabla);

    reservarMemoria(&memoria, config.sizeM);
    cargarMemoria(config.archivoVMX, memoria, config, hdr);

    inicializarRegistros(registros, indices);
    imprimirRegistros("Registros iniciales:", registros);

    printf("\nEjecutando programa...\n");
    ejecutarPrograma(tabla, memoria, registros, config.flagD, indices);

    imprimirRegistros("Registros al finalizar:", registros);
    return 0;
}