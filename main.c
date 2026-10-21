#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "include/consola.h"
#include "include/loader.h"
#include "include/tablaSegmentos.h"
#include "include/memoria.h"
#include "include/registros.h"
#include "include/cpu.h"

int main(int argc, char *argv[]) {
    Config cfg;
    int i;
    uint16_t code_size;
    uint8_t version;
    tabla_segmentos t;
    Memoria m;
    Registros r;

    parsearArgumentos(argc, argv, &cfg);

    //--- borrar cartel
    printf("Archivo vmx: %s\n", cfg.archivoVMX);
    printf("Archivo vmi: %s\n", cfg.archivoVMI);

    printf("Memoria: %s\n", cfg.flagM ? "si" : "no");
    if(cfg.flagM) printf("tamanio solicitado: %d\n",cfg.sizeM);
    printf("Desensamblar: %s\n", cfg.flagD ? "si" : "no");
    printf("Parametros p: %s\n", cfg.flagP ? "si" : "no");
    for(i=0;i<cfg.nP;i++)
        printf("\t p_%02d : %d\n",i,cfg.p[i]);

    procesarHeader(cfg.archivoVMX,&version,&code_size);
    //--- borrar cartel
    printf("Version: %d \n",version);
    printf("tamaño del codigo: %X\n",code_size);

    cargarTablaSegmentos(t,code_size);
    //--borrar cartel
    printf("\n----------------------------------------");
    printf("\n num segmento \t base \t tamanio ");
    for(i=0;i<CANT_SEG;i++){
        printf("\n----------------------------------------");
        printf("\n\t %d \t %X \t %X ",i,obtenerBaseSegmento(t,i),obtenerTamanioSegmento(t,i));
    }

    reservarMemoria(&m,cfg.flagM?cfg.sizeM:TAMANIO_MEMORIA);
    cargarMemoria(cfg.archivoVMX,m,code_size,cfg.flagM?cfg.sizeM:TAMANIO_MEMORIA);
    
    inicializarRegistros(r);
    //--- borrar cartel
    printf("\n----------------------------------------");
    printf("\n num registro \t valor ");
    for(i=0;i<CANT_REGISTROS;i++){
        printf("\n----------------------------------------");
        printf("\n\t %d \t %08X",i,r[i]);
    }

    printf("\n---------------------------------------------\n");
    printf("Ejecutando programa...\n");
    ejecutarPrograma(t, m, r,cfg.flagD);
 
    printf("\n---------------------------------------------\n");
    printf("Registros al finalizar:\n");
    for (i = 0; i < CANT_REGISTROS; i++){
        printf("%d \t\t %08X\n", i, r[i]);
    }
    return 0;
}