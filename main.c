#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "consola.h"
#include "loader.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "cpu.h"

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

    /*
    // buscar como mostrar y verificar el proceso de buscar instruccion, decodificar, ejecutar
    printf("\n---------------------------------------------\n");
    printf("Primer fetch (buscarInstruccion):\n");
    Instruccion instr = buscarInstruccion(t, m, r);
    printf("opcode: 0x%02X\n", instr.opcode);
    printf("tipo operando A: %d\n", instr.operandoA.tipo);
    printf("tipo operando B: %d\n", instr.operandoB.tipo);
    printf("IP despues del fetch: 0x%08X\n", r[REGIP]);
 
    if (instr.operandoA.tipo != TIPO_NINGUNO){
        uint32_t valorA = leerValorOperando(instr.operandoA, t, m, r);
        printf("valor resuelto operando A: 0x%08X\n", valorA);
    }
    if (instr.operandoB.tipo != TIPO_NINGUNO){
        uint32_t valorB = leerValorOperando(instr.operandoB, t, m, r);
        printf("valor resuelto operando B: 0x%08X\n", valorB);
    }
    */

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