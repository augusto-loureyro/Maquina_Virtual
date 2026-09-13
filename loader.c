#include "include/loader.h"

void procesarHeader(const char *nombreArchivo,uint8_t *version,uint16_t *code_size){
        FILE *archVMX;
        uint8_t header[8];

        if((archVMX = fopen(nombreArchivo,"rb"))!=NULL){
            if(fread(header, 1, 8, archVMX) == 8) {
                if(memcmp(header,"VMX26",5)!=0){ reportarError(ERROR_ENCABEZADO_INVALIDO);} //El archivo sample.vmx es viejo, dice VMX25

                *version=header[5];
                if(*version!=1){ reportarError(ERROR_VERSION_NO_SOPORTADA);}
                
                *code_size = (header[6] << 8) | header[7]; // big-endian
            }

            fclose(archVMX);
        }
}

void leerCodigo(){
    printf("\n \n paso siguiente: leer codigo");
}