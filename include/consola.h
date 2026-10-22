#ifndef CONSOLA_H
#define CONSOLA_H

/**
 * Formato esperado:
 *   vmx <archivo.vmx> [-d]
 *   (admite también el flag -d antes o después del nombre de archivo,
 *    y tolera la invocación directa o vía prefijo 'vmx').
 *
 */
void parsearArgumentos(int argc, char *argv[], char **nombreArchivo, int *flagD);

#endif // CONSOLA_H
