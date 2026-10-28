#include <stdio.h>
#include <stdint.h>
#include "consola.h"
#include "loader.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "cpu.h"

int main(int argc, char *argv[]) {
    ConfiguracionMV config;
    tabla_segmentos tablaSegmentos;
    Memoria memoria = NULL;
    Registros registros;

    /// 1. Inicializar y parsear argumentos de linea de comandos
    inicializarConfiguracion(&config);
    parsearArgumentos(argc, argv, &config);

    /*
        CASO 0 -> ni vmx, ni vmi
        CASO 1 -> solo vmi
        CASO 2 -> solo vmx
        CASO 3 -> vmx y vmi

        CASO 0, ya da error
        CASO 1, se ejecuta solo lo de vmi
        CASO 2 y CASO 3, ejecución ya implementada (falta el breakpoint); en el caso 3, el breakpoint no hace nada
    */ 

    //CASO 0 -> ya lo reporta como error parsearArgumentos
    /* CASO 1
            Si no recibe un .vmx y recibe solo un archivo de imagen .vmi, debe cargar la memoria principal, la 
        tabla de segmentos y los registros tal como están en el archivo y continuar la ejecución.
    */
    if(config.archVMI != NULL && config.archVMX == NULL){ 

        procesarHeaderVMI(config.archVMI, &config.headerVMI);
        uint32_t tamanioRAM = config.headerVMI.mem_kib * BYTES_POR_KIB;

        uint8_t bytesRegistros[BYTES_REGISTROS_VMI];
        uint8_t bytesTabla[BYTES_TABLA_VMI];

        leerRegistrosYTablaVMI(config.archVMI, bytesRegistros, bytesTabla);
        cargarRegistrosVMI(bytesRegistros, registros);
        cargarTablaSegmentosVMI(bytesTabla, tablaSegmentos);

        reservarMemoria(&memoria, tamanioRAM);
        cargarMemoriaVMI(config.archVMI, memoria, tamanioRAM);

        mostrarTablaSegmentos(tablaSegmentos);
        mostrarRegistros(registros);

        ///// CONTINÚA EJECUCIÓN NORMAL
    } else if (config.archVMX != NULL) { // CASO 2 y CASO 3
        procesarHeader(config.archVMX, &config.header);

        /// Si es VMX version 2, procesar memoria y segmentos
        if (config.header.version == 2) {
            uint32_t tamanioRAM = config.mem_kib * BYTES_POR_KIB;

            /// Mostrar metadatos del encabezado y configuracion
            mostrarMetadatos(&config);

            /// Construir tabla de segmentos 
            cargarTablaSegmentosV2(tablaSegmentos, &config);

            /// Reservar memoria RAM de tamano variable segun m=M
            reservarMemoria(&memoria, tamanioRAM);

            /// Inicializar registros V2 (CS, DS, ES, SS, KS, PS, SP, IP)
            inicializarRegistrosV2(registros, tablaSegmentos, &config);

            /// Cargar en memoria el Param Segment si existe (-p)
            cargarParamSegment(&config, memoria, tablaSegmentos, registros);

            /// Cargar el Code Segment y el Const Segment
            cargarMemoriaV2(config.archVMX, memoria, tamanioRAM, tablaSegmentos, registros, &config.header);

            /// Mostrar informacion de la tabla y los registros de segmentos
            mostrarTablaSegmentos(tablaSegmentos);
            mostrarRegistros(registros);
        } else {
            /// 3. Flujo MV1 (version 1) para asegurar retrocompatibilidad y no regresion:
            cargarTablaSegmentosV1(tablaSegmentos, config.header.code_size);

            /// 4. Reservar memoria RAM y cargar el segmento de codigo
            reservarMemoria(&memoria, TAMANIO_MEMORIA);
            cargarMemoriaV1(config.archVMX, memoria, config.header.code_size);

            /// 5. Inicializar el banco de 32 registros 
            inicializarRegistrosV1(registros);
        }
    }

    /// 6. Ejecutar el ciclo de instruccion (común a todos los casos)
    ejecutarPrograma(tablaSegmentos, memoria, registros, config.flagD); //por ahora solo config.flagD

    /// 7. Liberar recursos antes de salir
    liberarMemoria(&memoria);

    return 0;
}
