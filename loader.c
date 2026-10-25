#include "include/loader.h"
#include "include/memoria.h"

static int leerUint16BigEndian(FILE *archivo, uint16_t *valor) {
    uint8_t bytes[2];

    if (fread(bytes, sizeof(bytes[0]), sizeof(bytes), archivo) != sizeof(bytes)) {
        return 0;
    }

    *valor = ((uint16_t)bytes[0] << 8) | bytes[1];
    return 1;
}

void procesarHeaderVMX(const char *nombreArchivo, Header *hdr) {
    FILE *archivo;
    uint8_t id[5];

    if (nombreArchivo == NULL || hdr == NULL) {
        reportarError(ERROR_ARGUMENTOS_INVALIDOS);
    }

    memset(hdr, 0, sizeof(*hdr));

    archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL) {
        reportarError(ERROR_ARCHIVO_NO_ENCONTRADO);
    }

    if (fread(id, sizeof(id[0]), sizeof(id), archivo) != sizeof(id) ||
        fread(&hdr->version, sizeof(hdr->version), 1, archivo) != 1) {
        fclose(archivo);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    if (memcmp(id, "VMX26", sizeof(id)) != 0) {
        fclose(archivo);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    if (hdr->version != 1 && hdr->version != 2) {
        fclose(archivo);
        reportarError(ERROR_VERSION_NO_SOPORTADA);
    }

    if (hdr->version == 1) {
        if (!leerUint16BigEndian(archivo, &hdr->sizeCS) ||
            hdr->sizeCS > TAMANIO_MEMORIA_DEFAULT) {
            fclose(archivo);
            reportarError(ERROR_ENCABEZADO_INVALIDO);
        }

        hdr->sizeDS = TAMANIO_MEMORIA_DEFAULT - hdr->sizeCS;
    } else if (!leerUint16BigEndian(archivo, &hdr->sizeCS) ||
               !leerUint16BigEndian(archivo, &hdr->sizeDS) ||
               !leerUint16BigEndian(archivo, &hdr->sizeES) ||
               !leerUint16BigEndian(archivo, &hdr->sizeSS) ||
               !leerUint16BigEndian(archivo, &hdr->sizeKS) ||
               !leerUint16BigEndian(archivo, &hdr->offsetEP)) {
        fclose(archivo);
        reportarError(ERROR_ENCABEZADO_INVALIDO);
    }

    fclose(archivo);
}