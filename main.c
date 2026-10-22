#include <stdio.h>
#include <stdint.h>
#include "consola.h"
#include "loader.h"
#include "tablaSegmentos.h"
#include "memoria.h"
#include "registros.h"
#include "cpu.h"

int main(int argc, char *argv[]) {
    char *nombreArchivo = NULL;
    int flagD = 0;
    uint8_t version = 0;
    uint16_t code_size = 0;

    tabla_segmentos t;
    Memoria m = NULL;
    Registros r;

    /// 1. Parsear y validar argumentos de linea de comandos: vmx <archivo.vmx> [-d]
    parsearArgumentos(argc, argv, &nombreArchivo, &flagD);

    /// 2. Procesar y validar encabezado del ejecutable (identificador VMX26, version y code_size)
    procesarHeader(nombreArchivo, &version, &code_size);

    /// 3. Inicializar y cargar la tabla de descriptores de segmentos (CODE y DATA)
    cargarTablaSegmentos(t, code_size);

    /// 4. Reservar 16 KiB de memoria RAM y cargar el segmento de codigo
    reservarMemoria(&m, TAMANIO_MEMORIA);
    cargarMemoria(nombreArchivo, m, code_size);

    /// 5. Inicializar el banco de 32 registros (CS, DS, IP apuntando a CODE)
    inicializarRegistros(r);

    /// 6. Ejecutar el ciclo de instrucción de la Máquina Virtual
    ejecutarPrograma(t, m, r, flagD);

    /// 7. Liberar recursos antes de finalizar
    liberarMemoria(&m);

    return 0;
}
