#include <stdio.h>
#include <stdint.h>
#include "consola.h"
#include "loader.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "cpu.h"
#include "pila.h"

int main(int argc, char *argv[]) {
    ConfiguracionMV config;
    tabla_segmentos tablaSegmentos;
    Memoria memoria = NULL;
    Registros registros;

    /// 1. Inicializar y parsear argumentos de linea de comandos
    inicializarConfiguracion(&config);
    parsearArgumentos(argc, argv, &config);

    /*
        CASO 0 -> ni vmx, ni vmi (reportado como error en parsearArgumentos)
        CASO 1 -> solo vmi (reanudacion autonoma desde archivo de imagen)
        CASO 2 -> solo vmx (ejecucion normal; breakpoint SYS 0xF ignorado)
        CASO 3 -> vmx y vmi (ejecucion con depuracion; SYS 0xF genera y actualiza vmi)
    */ 

    //CASO 0 -> ya lo reporta como error parsearArgumentos
    /* CASO 1
            Si no recibe un .vmx y recibe solo un archivo de imagen .vmi, debe cargar la memoria principal, la 
        tabla de segmentos y los registros tal como están en el archivo y continuar la ejecución.
    */
    if(config.archVMI != NULL && config.archVMX == NULL){ 
        procesarHeaderVMI(config.archVMI, &config.headerVMI);
        uint32_t tamanioRAM = config.headerVMI.mem_kib * BYTES_POR_KIB;
        config.mem_kib = config.headerVMI.mem_kib;

        uint8_t bytesRegistros[BYTES_REGISTROS_VMI];
        uint8_t bytesTabla[BYTES_TABLA_VMI];

        leerRegistrosYTablaVMI(config.archVMI, bytesRegistros, bytesTabla);
        cargarRegistrosVMI(bytesRegistros, registros);
        cargarTablaSegmentosVMI(bytesTabla, tablaSegmentos);

        reservarMemoria(&memoria, tamanioRAM);
        cargarMemoriaVMI(config.archVMI, memoria, tamanioRAM);
    }/// CASO 2 y 3: Archivo ejecutable (.vmx), con o sin archivo de imagen (.vmi)
    else if (config.archVMX != NULL) {
        procesarHeader(config.archVMX, &config.header);

        /// Flujo MV2 (version 2): hasta seis segmentos, memoria variable y pila
        if (config.header.version == 2) {
            uint32_t tamanioRAM = config.mem_kib * BYTES_POR_KIB;

            /// Construir tabla de descriptores de segmentos
            cargarTablaSegmentosV2(tablaSegmentos, &config);

            /// Reservar memoria RAM de tamano variable segun m=M
            reservarMemoria(&memoria, tamanioRAM);

            /// Inicializar registros V2 (CS, DS, ES, SS, KS, PS, SP, IP)
            inicializarRegistrosV2(registros, tablaSegmentos, &config);

            /// Cargar en memoria el Param Segment si existe (-p) y preparar la pila de main
            uint32_t argcSubrutina = 0;
            uint32_t argvSubrutina = 0xFFFFFFFF;
            cargarParamSegment(&config, memoria, tablaSegmentos, registros, &argvSubrutina, &argcSubrutina);
            inicializarPilaSubrutinaPrincipal(memoria, tablaSegmentos, registros, argcSubrutina, argvSubrutina);

            /// Cargar el Code Segment y el Const Segment
            cargarMemoriaV2(config.archVMX, memoria, tamanioRAM, tablaSegmentos, registros, &config.header);
        } else {
            /// Flujo MV1 (version 1) para retrocompatibilidad total
            cargarTablaSegmentosV1(tablaSegmentos, config.header.code_size);

            /// Reservar 16 KiB de RAM y cargar el codigo en base 0
            reservarMemoria(&memoria, TAMANIO_MEMORIA);
            cargarMemoriaV1(config.archVMX, memoria, config.header.code_size);

            /// Inicializar el banco de registros para MV1
            inicializarRegistrosV1(registros);
        }
    }

    /// Ejecutar el programa (comun a todos los flujos)
    ejecutarPrograma(tablaSegmentos, memoria, registros, &config);

    /// Liberar recursos antes de salir
    liberarMemoria(&memoria);

    return 0;
}
