#include"errores.h"

void reportarError(TipoError tipo) {
    const char *mensaje;
    switch (tipo) {
        case ERROR_ARCHIVO_NO_ENCONTRADO: mensaje = "No se pudo abrir el archivo"; break;
        case ERROR_ENCABEZADO_INVALIDO:   mensaje = "Encabezado inválido"; break;
        case ERROR_VERSION_NO_SOPORTADA:  mensaje = "Versión no soportada"; break;
        case ERROR_INSTRUCCION_INVALIDA:  mensaje = "Instrucción inválida"; break;
        case ERROR_DIVISION_POR_CERO:     mensaje = "División por cero"; break;
        case ERROR_FALLO_DE_SEGMENTO:     mensaje = "Fallo de segmento"; break;
        default:                          mensaje = "Error desconocido"; break;
    }
    fprintf(stderr, "Error: %s \n", mensaje);
    exit(tipo + 1); // código de salida distinto según el tipo de error
}