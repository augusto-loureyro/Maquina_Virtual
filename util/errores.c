#include "errores.h"

void reportarError(TipoError tipo) {
    const char *mensaje;

    switch (tipo) {
        case ERROR_ARGUMENTOS_INVALIDOS:
            mensaje = "Argumentos invalidos. Uso: vmx <archivo.vmx> [-d]";
            break;
        case ERROR_ARCHIVO_NO_ENCONTRADO:
            mensaje = "No se pudo abrir el archivo";
            break;
        case ERROR_ENCABEZADO_INVALIDO:
            mensaje = "Encabezado invalido o archivo truncado";
            break;
        case ERROR_VERSION_NO_SOPORTADA:
            mensaje = "Version no soportada";
            break;
        case ERROR_INSTRUCCION_INVALIDA:
            mensaje = "Instruccion invalida";
            break;
        case ERROR_DIVISION_POR_CERO:
            mensaje = "Division por cero";
            break;
        case ERROR_FALLO_DE_SEGMENTO:
            mensaje = "Fallo de segmento";
            break;
        case ERROR_TAMANIO_SYS:
            mensaje = "Tamanio de syscall invalido";
            break;
        case ERROR_MODO_SYS:
            mensaje = "Modo de syscall invalido";
            break;
        case ERROR_SYS_INVALIDA:
            mensaje = "Llamada al sistema invalida";
            break;
        case ERROR_ENTRADA_SYS:
            mensaje = "Entrada de syscall invalida o fin de archivo (EOF)";
            break;
        case ERROR_MEMORIA_INSUFICIENTE:
            mensaje = "No se pudo asignar memoria para la RAM";
            break;
        default:
            mensaje = "Error desconocido";
            break;
    }

    fprintf(stderr, "Error: %s\n", mensaje);
    exit(tipo + 1);
}
