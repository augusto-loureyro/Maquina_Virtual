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

        /// Si es VMX version 2, se reconocen y muestran los metadatos
        if (config.header.version == 2) {
            mostrarMetadatos(&config);
            return 0;
        }
    } else {
        /// Si solo se especifico un archivo .vmi, se muestra la configuracion
        mostrarConfiguracion(&config);
        return 0;
    }

    /// 3. Flujo MV1 (version == 1) para asegurar retrocompatibilidad y no regresion:
    cargarTablaSegmentos(t, config.header.code_size);

    /// 4. Reservar memoria RAM y cargar el segmento de codigo
    reservarMemoria(&m, TAMANIO_MEMORIA);
    cargarMemoria(config.archVMX, m, config.header.code_size);

    /// 5. Inicializar el banco de 32 registros (CS, DS, IP apuntando a CODE)
    inicializarRegistros(r);

    /// 6. Ejecutar el ciclo de instruccion de la Maquina Virtual
    ejecutarPrograma(t, m, r, config.flagD);

    /// 7. Liberar recursos antes de finalizar
    liberarMemoria(&m);

    return 0;
}
