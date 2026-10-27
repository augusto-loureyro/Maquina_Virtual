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

    /// 2. Procesar encabezado del ejecutable si se indico archivo .vmx
    if (config.archVMX != NULL) {
        procesarHeader(config.archVMX, &config.header);

        /// Si es VMX version 2, procesar memoria y segmentos
        if (config.header.version == 2) {
            uint32_t tamanioRAM = config.mem_kib * 1024;

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

            /// Liberar recursos antes de salir
            liberarMemoria(&memoria);
            return 0;
        }
    } else {
        /// Si solo se especifico un archivo .vmi, se muestra la configuracion
        mostrarConfiguracion(&config);
        return 0;
    }

    /// 3. Flujo MV1 (version 1) para asegurar retrocompatibilidad y no regresion:
    cargarTablaSegmentosV1(tablaSegmentos, config.header.code_size);

    /// 4. Reservar memoria RAM y cargar el segmento de codigo
    reservarMemoria(&memoria, TAMANIO_MEMORIA);
    cargarMemoriaV1(config.archVMX, memoria, config.header.code_size);

    /// 5. Inicializar el banco de 32 registros 
    inicializarRegistrosV1(registros);

    /// 6. Ejecutar el ciclo de instruccion 
    ejecutarPrograma(tablaSegmentos, memoria, registros, config.flagD);

    /// 7. Liberar recursos antes de salir
    liberarMemoria(&memoria);

    return 0;
}
