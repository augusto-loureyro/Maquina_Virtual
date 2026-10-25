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
    tabla_segmentos t;
    Memoria m = NULL;
    Registros r;

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

            /// Construir tabla de descriptores de segmentos 
            cargarTablaSegmentosV2(t, &config);

            /// Reservar memoria RAM de tamano variable segun m=M
            reservarMemoria(&m, tamanioRAM);

            /// Inicializar registros V2 (CS, DS, ES, SS, KS, PS, SP, IP)
            inicializarRegistrosV2(r, t, &config);

            /// Cargar en memoria el Param Segment si existe (-p)
            cargarParamSegment(&config, m, t, r);

            /// Cargar el Code Segment y el Const Segment
            cargarMemoriaV2(config.archVMX, m, tamanioRAM, t, r, &config.header);

            /// Mostrar informacion de la tabla y los registros de segmentos
            mostrarTablaSegmentos(t);
            mostrarRegistros(r);

            /// Liberar recursos antes de salir
            liberarMemoria(&m);
            return 0;
        }
    } else {
        /// Si solo se especifico un archivo .vmi, se muestra la configuracion
        mostrarConfiguracion(&config);
        return 0;
    }

    /// 3. Flujo MV1 (version 1) para asegurar retrocompatibilidad y no regresion:
    cargarTablaSegmentosV1(t, config.header.code_size);

    /// 4. Reservar memoria RAM y cargar el segmento de codigo
    reservarMemoria(&m, TAMANIO_MEMORIA);
    cargarMemoriaV1(config.archVMX, m, config.header.code_size);

    /// 5. Inicializar el banco de 32 registros 
    inicializarRegistrosV1(r);

    /// 6. Ejecutar el ciclo de instruccion 
    ejecutarPrograma(t, m, r, config.flagD);

    /// 7. Liberar recursos antes de salir
    liberarMemoria(&m);

    return 0;
}
