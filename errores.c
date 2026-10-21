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
        case ERROR_ARGUMENTOS_INVALIDOS:  mensaje = "comando invalido"; break;
        default:                          mensaje = "Error desconocido"; break;
    }
    fprintf(stderr, "Error: %s \n", mensaje);
    exit(tipo + 1); // código de salida distinto según el tipo de error
}

// vista la forma de probar, los errores pueden separarse,
// pero debe reconocerse si pertenece a uno de estos:
/*

● Instrucción inválida: cuando el código de operación de la instrucción a ejecutar no existe. 
● División por cero: cuando al ejecutar la instrucción DIV, el valor del segundo operando es 0. 
● Fallo de segmento: cuando el código de segmento de una dirección lógica sea inválido (excede el 
tamaño de la tabla de descriptores o la entrada tiene el valor -1) o cuando la dirección física 
apunta a un byte que se encuentra fuera de los límites del segmento, excepto en el caso de la 
lectura de la instrucción. 

● Memoria insuficiente: cuando la memoria principal no cuenta con espacio suficiente para alojar 
todos los segmentos del proceso. 
● Stack overflow: cuando se ejecuta una instrucción PUSH o CALL y no haya espacio suficiente en 
la pila. 
● Stack underflow: cuando se ejecuta una instrucción POP o RET y la pila ya está vacía o no puede 
extraer los 4 bytes correspondientes. 
Ante la ocurrencia de cualquiera de estos errores, la máquina virtual debe informarlo e 
inmediatamente abortar la ejecución del proceso. 
*/