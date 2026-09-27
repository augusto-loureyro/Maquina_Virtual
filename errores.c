// centraliza el manejo de errores; se puede agregar detalle.

#include"include/errores.h"

void reportarError(TipoError tipo) {
    const char *mensaje;
    switch (tipo) {
        case ERROR_ARCHIVO_NO_ENCONTRADO: mensaje = "No se pudo abrir el archivo"; break;
        case ERROR_ENCABEZADO_INVALIDO:   mensaje = "Encabezado invalido"; break;
        case ERROR_VERSION_NO_SOPORTADA:  mensaje = "Version no soportada"; break;
        case ERROR_INSTRUCCION_INVALIDA:  mensaje = "Instruccion invalida"; break;
        case ERROR_DIVISION_POR_CERO:     mensaje = "Division por cero"; break;
        case ERROR_FALLO_DE_SEGMENTO:     mensaje = "Fallo de segmento"; break;
        case ERROR_TAMANIO_SYS:           mensaje = "Tamanio de syscall invalido"; break;
        case ERROR_ENTRADA_SYS:           mensaje = "Entrada de syscall invalida"; break;
        default:                          mensaje = "Error desconocido"; break;
    }
    fprintf(stderr, "Error: %s \n", mensaje);
    exit(tipo + 1); // código de salida distinto según el tipo de error
}