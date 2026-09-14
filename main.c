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
    char *nombreArchivo;
    int flagD,i;
    uint16_t code_size;
    uint8_t version;
    tabla_segmentos t;
    Memoria m;
    Registros r;

    parsearArgumentos(argc, argv, &nombreArchivo, &flagD);
    //--- borrar cartel
    printf("Archivo: %s\n", nombreArchivo);
    printf("Desensamblar: %s\n", flagD ? "si" : "no");

    procesarHeader(nombreArchivo,&version,&code_size);
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

    reservarMemoria(&m,TAMANIO_MEMORIA);
    cargarMemoria(nombreArchivo,m);
    
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
    ejecutarPrograma(t, m, r);
 
    printf("\n---------------------------------------------\n");
    printf("Registros al finalizar:\n");
    for (i = 0; i < CANT_REGISTROS; i++){
        printf("%d \t\t %08X\n", i, r[i]);
    }
    return 0;
}