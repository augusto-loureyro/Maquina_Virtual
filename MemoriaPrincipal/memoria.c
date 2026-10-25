#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memoria.h"
#include "errores.h"

void reservarMemoria(Memoria *m,int sizeM){ //sizeM, en KiB, vale lo que se indicó en el comando, o 16 si no se indicó
    *m = (uint8_t *)calloc(sizeM*BYTES_POR_KIB, 1); //reservo sizeM*1024 bytes inicializados en 0...
}

void cargarMemoria(char *nombreArchivo, Memoria m, Config cfg, Header hdr){
    FILE *archVMX;

    if ((archVMX = fopen(nombreArchivo, "rb")) == NULL){
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    if(hdr.version==1)
        fseek(archVMX, 8, SEEK_SET);
    else if(hdr.version==2)
        fseek(archVMX, 18, SEEK_SET);
    else 
        reportarError(ERROR_VERSION_NO_SOPORTADA);

    /*
            Todos los segmentos deben cargarse de forma contigua en la memoria, 
        respetando el siguiente orden: Param Segment, Const Segment, Code 
        Segment, Data Segment, Extra Segment, Stack Segment. Sin embargo, a 
        excepción del Code Segment y del Stack Segment, puede suceder que no 
        todos los segmentos estén presentes. 
    */
    if(cfg.flagP && cfg.nP>0){
        cargarParamSegment(cfg.archivoVMX,m,(size_t)cfg.sizeM*BYTES_POR_KIB,cfg.nP,cfg.p,&hdr.sizePS);
    }

    size_t offset=hdr.sizePS;
    if(fread(m+offset,1,hdr.sizeKS,archVMX)!=hdr.sizeKS){
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO); // archivo truncado: menos bytes de los que declara el header
    }
    offset+=hdr.sizeKS;
    if(fread(m+offset,1,hdr.sizeCS,archVMX)!=hdr.sizeCS){
        fclose(archVMX);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    fclose(archVMX);
}

/*
El Param Segment es un segmento que se crea según los parámetros de ejecución, si es que se incluye 
el flag -p en la línea de comandos. De existir, siempre se ubicará a partir de la dirección física 0x00000000 
de la memoria, dado que será el primer segmento del proceso y la máquina virtual solo ejecuta un único 
proceso; por lo tanto, estará definido en la posición 0 de la tabla de segmentos. 
El contenido del segmento será: primero, el nombre del archivo .vmx y todas las cadenas de caracteres 
(strings) de los parámetros; al final, un arreglo argv de tamaño argc de punteros de 4 bytes a cada 
posición de inicio de los strings de cada parámetro, siendo argc la cantidad de parámetros. Los punteros 
contendrán en los 16 bits menos significativos el offset de cada string dentro del segmento y en los 16 bits 
más significativos 0x0000, la entrada a la tabla de segmentos.
*/

void cargarParamSegment(char *archVMX,Memoria m,size_t capacidad,int argc,char *p[],uint16_t *sizePS){
    if(archVMX==NULL || m==NULL || argc<0 || (argc>0 && p==NULL) || sizePS==NULL){
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    size_t offset=strlen(archVMX)+1;
    if(offset>capacidad || offset>UINT16_MAX){
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }

    for(int i=0;i<argc;i++){
        if(p[i]==NULL){
            reportarError(ERROR_ARGUMENTOS_INVALIDOS);
        }
        size_t bytesParametro=strlen(p[i])+1;
        if(bytesParametro>capacidad-offset || bytesParametro>UINT16_MAX-offset){
            reportarError(ERROR_MEMORIA_INSUFICIENTE);
        }
        offset+=bytesParametro;
    }

    size_t bytesArgv=(size_t)argc*4;
    if(bytesArgv>capacidad-offset || bytesArgv>UINT16_MAX-offset){
        reportarError(ERROR_MEMORIA_INSUFICIENTE);
    }

    memcpy(m,archVMX,strlen(archVMX)+1);

    size_t offsetParametro=strlen(archVMX)+1;
    for(int i=0;i<argc;i++){
        size_t bytesParametro=strlen(p[i])+1;
        memcpy(m+offsetParametro,p[i],bytesParametro);
        offsetParametro+=bytesParametro;
    }

    size_t offsetArgv=offset;
    offsetParametro=strlen(archVMX)+1;
    for(int i=0;i<argc;i++){
        m[offsetArgv++]=0;
        m[offsetArgv++]=0;
        m[offsetArgv++]=(uint8_t)(offsetParametro>>8);
        m[offsetArgv++]=(uint8_t)offsetParametro;
        offsetParametro+=strlen(p[i])+1;
    }

    *sizePS=(uint16_t)(offset+bytesArgv);
}

/// MBR -> al leer de, y al escribir en, memoria, se actualiza con el valor a escribir/leer
uint32_t leerDeMemoria(Memoria m, uint32_t f, int n, Registros r){
    uint32_t valor = 0;
    for (int k = 0; k < n; k++)
        valor = (valor << 8) | m[f + k];    /// big-endian, n bytes
    r[REGMBR] = valor;
    return valor;
}
void escribirEnMemoria(Memoria m, uint32_t f, uint32_t valor, int n, Registros r){
    r[REGMBR] = valor;                       
    for (int k = 0; k < n; k++)
        m[f + k] = (valor >> (8 * (n - 1 - k))) & 0xFF;
}