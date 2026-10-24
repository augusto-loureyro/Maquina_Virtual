#ifndef ERRORES_H
#define ERRORES_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    ERROR_ARGUMENTOS_INVALIDOS,   /**< Argumentos de línea de comandos inválidos */
    ERROR_ARCHIVO_NO_ENCONTRADO,   /**< No se pudo abrir el archivo especificado */
    ERROR_ENCABEZADO_INVALIDO,     /**< El archivo no tiene encabezado VMX26 válido o está truncado */
    ERROR_VERSION_NO_SOPORTADA,    /**< Version de formato VMX distinta de 1 o 2 */
    ERROR_INSTRUCCION_INVALIDA,    /**< Código de operación inexistente o tipo de operando no permitido */
    ERROR_DIVISION_POR_CERO,       /**< Intento de división con segundo operando igual a 0 */
    ERROR_FALLO_DE_SEGMENTO,       /**< Dirección lógica inválida o acceso fuera de los límites del segmento */
    ERROR_TAMANIO_SYS,             /**< Tamaño de celda en llamada al sistema fuera de rango [1..4] */
    ERROR_MODO_SYS,                /**< Modo de lectura/escritura de syscall no soportado */
    ERROR_SYS_INVALIDA,            /**< Número de syscall distinto de READ (1) o WRITE (2) */
    ERROR_ENTRADA_SYS,             /**< Error o fin de archivo (EOF) al leer entrada en syscall READ */
    ERROR_MEMORIA_INSUFICIENTE     /**< Fallo al reservar memoria para la RAM */
} TipoError;

void reportarError(TipoError tipo);

#endif // ERRORES_H
