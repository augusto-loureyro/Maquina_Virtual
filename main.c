/*
sebas: 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "maquinaVirtual.h"

#define ARCHT 100

/// argv[0] = vmx
/// argv[1] = programa.vmx
/// argv[2] = -d   (Opcional)

int main(int argc, char *argv[]) {
    ETMaquinaVirtual maqVirt;
    char nombreArchivo[ARCHT];
    bool modoDebug = false;

    strcpy(nombreArchivo, argv[1]);

    if (strcmp(argv[2],"-d") == 0)
        modoDebug = true;

    mvInic(&maqVirt, modoDebug);
    cargarArchivo(&maqVirt, nombreArchivo);
    mvEjecutar(&maqVirt);
    return 0;
}

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> 
// ya existe cte FILENAME_MAX en stdio
/// argv[0] = vmx
/// argv[1] = programa.vmx
/// argv[2] = -d   (Opcional) -> disassembler

/*
 en la version 1, se espera: 
    comando: vmx filename.vmx [-d]
    --> entonces: argc==2 && *argv=["vmx","filename.vmx"] || argc==3 && *argv= ["vmx","filename.vmx","-d"]
    //debo comprobar que esto se de así y guardar la info necesaria.
*/

void procesarHeader(char nombrearchivo[FILENAME_MAX], uint8_t *version,uint16_t *code_size);

int main(int argc, char *argv[])
{
    char nombreArchivo[FILENAME_MAX];
    int flagD = 0;
    uint8_t version;
    uint16_t code_size;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s filename.vmx [-d]\n", argv[0]);
        return 1;
    }

    if(argc>=2 && strstr(argv[1],".vmx")){ 
        strcpy(nombreArchivo,argv[1]);
        flagD = (argc>=3 && strcmp("-d",argv[2])==0);
    } 

    procesarHeader(nombreArchivo,&version,&code_size); //no se si iría en main

    printf("Version: %u, code_size: %u, flagD=%d\n", version, code_size, flagD);

    return 0;
}

//esto iría en algún otro archivo
void procesarHeader(char nombrearchivo[FILENAME_MAX], uint8_t *version,uint16_t *code_size){
        FILE *archVMX;
        uint8_t header[8];

        if((archVMX = fopen(nombrearchivo,"rb"))!=NULL){
                if(fread(header, 1, 8, archVMX) == 8) {
                    if(memcmp(header,"VMX26",5)!=0){  printf("error 1");/* error, no coincide ident. */}

                    *version=header[5];
                    if(*version!=1){ printf("error 2");/* error, version incompatible*/}
                    
                    *code_size = (header[6] << 8) | header[7]; // big-endian
                }

                fclose(archVMX);
            }
}

/*para ejecutar: 
  primero gcc main.c -o vmx
  luego  vmx sample.vmx -d
*/