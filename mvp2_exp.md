# Máquina Virtual, grupo 0B, parte 2


## Flujo de la MV: desde los argumentos hasta la tabla de segmentos

El recorrido principal está en `main.c`. A grandes rasgos, primero se arma una configuración a partir
del comando, después se lee la cabecera del `.vmx`, se reserva y carga la memoria, y finalmente se
construye la tabla que permite ubicar cada segmento.

Por ejemplo, se puede invocar la máquina así:

```powershell
.\vmg programa.vmx m=32 -d -p hola mundo
```

En este ejemplo se solicita una memoria de 32 KiB, se activa el desensamblado y se pasan dos
parámetros al programa: `hola` y `mundo`.

### 1. Preparar y completar la configuración

En `main`, `inicializarConfiguracion` deja los campos en un estado conocido:

- Los nombres de los archivos `.vmx` y `.vmi` empiezan en `NULL`.
- La memoria queda en el valor por defecto: 16 KiB.
- Los flags y la cantidad de parámetros empiezan en cero.
- La cabecera se inicializa vacía.

Luego `parsearArgumentos` recorre los argumentos y completa esa misma estructura
`ConfiguracionMV`. Comprueba que haya por lo menos un archivo, que no se repita cada tipo de archivo,
que el valor de `m=M` sea válido y que `-p` aparezca al final, porque todo lo que lo sigue se toma
como parámetro del programa.

Los parámetros no se convierten a números: se guardan como cadenas (`char *`). Si se indica un `.vmi`
pero no un `.vmx`, la máquina muestra la configuración y termina; en esta etapa todavía no carga el
`.vmi`. Si no hay `.vmx`, también se descartan los parámetros de `-p`.

### 2. Leer y distinguir la cabecera

Con un `.vmx` disponible, `procesarHeader` abre el archivo y valida el identificador `VMX26`. Después
lee el byte de versión y, según su valor, interpreta la cantidad correspondiente de bytes:

| Versión | Tamaño de cabecera | Información de segmentos |
| --- | ---: | --- |
| 1 | 8 bytes | Tamaño del segmento de código |
| 2 | 18 bytes | Código, datos, extra, stack, constantes y punto de entrada |

Los tamaños de 16 bits se leen en orden big-endian: primero el byte más significativo y después el
menos significativo. Para la versión 1, los campos que no vienen en la cabecera quedan inicialmente
en cero. Si el identificador, la versión o la longitud de la cabecera no son válidos, se informa el
error y se aborta.

### 3. Determinar cuánto espacio se va a usar

La versión 1 mantiene la memoria fija en 16 KiB, aunque el comando incluya `m=M`. La versión 2 usa el
tamaño indicado por `m=M`, o los 16 KiB predeterminados si no se indicó.

Antes de reservar memoria se comprueba que los tamaños de segmentos que ya se conocen quepan en la
memoria solicitada y que el espacio direccionable por los offsets de segmento no se exceda. El tamaño
del segmento de parámetros todavía es cero en este punto: se calcula al construirlo, más adelante
durante la carga.

### 4. Reservar memoria y cargar los segmentos

`reservarMemoria` solicita `mem_kib * 1024` bytes con `calloc`. Además de reservar el bloque, `calloc` lo deja inicializado en cero; esto permite que las partes de memoria que no se copian directamente desde el archivo comiencen vacías.

Después, `cargarMemoria` vuelve a abrir el `.vmx` y salta la cabecera: 8 bytes para v1 o 18 para v2.
Si se pidieron parámetros, `cargarParamSegment` construye primero el Param Segment en la memoria:

1. Copia el nombre del archivo `.vmx` terminado en `\0`.
2. Copia cada parámetro, también terminado en `\0`.
3. Al final agrega un arreglo de entradas de 4 bytes, una por parámetro. Cada entrada contiene el
   offset de la cadena correspondiente dentro del segmento.

El tamaño resultante se guarda en `header.param_size`. Este segmento ocupa el comienzo de la memoria.
Si no se pasaron parámetros, su tamaño queda en cero y no se agrega a la tabla.

Para v1, una vez conocido el tamaño real de los parámetros, se calcula el segmento de datos como el
espacio restante de la memoria:

```text
tamaño de datos = memoria en bytes - tamaño del código - tamaño de parámetros
```

La carga continúa copiando desde el archivo el segmento de constantes y luego el de código, detrás
del Param Segment. En v1 no hay segmento de constantes, así que ese tamaño es cero. En v2 los otros
segmentos declarados en la cabecera (datos, extra y stack) forman parte del espacio reservado, que
inicialmente queda en cero; esta rutina copia desde el archivo las constantes y el código.

Una vez calculado el tamaño del Param Segment, `cargarMemoria` vuelve a comprobar que la suma de todos los segmentos quepan en la memoria y en el rango representable por los offsets. Si el archivo declara más bytes de código o constantes de los que realmente contiene, también se informa un encabezado inválido.

### 5. Construir la tabla de segmentos

Con los tamaños finales ya disponibles, `cargarTablaSegmentos` crea la tabla. Primero marca sus ocho
entradas como vacías (`0xFFFFFFFF`) e inicializa los índices de cada tipo de segmento como ausentes.
Luego recorre los segmentos en el mismo orden en que se ubican en memoria:

```text
Param → Const → Code → Data → Extra → Stack
```

Solo agrega una entrada si el tamaño del segmento es mayor que cero. Cada entrada de 32 bits contiene:

```text
bits altos (16) : base física del segmento
bits bajos (16) : tamaño del segmento en bytes
```

La base de la primera entrada presente es cero. Cada segmento siguiente comienza justo después del
anterior, por lo que las bases se obtienen acumulando sus tamaños. Por ejemplo, si el Param Segment
mide 20 bytes y el de constantes mide 4 bytes, sus entradas empiezan en las bases 0 y 20; el siguiente segmento presente empieza en la base 24.

Además de llenar la tabla, se guarda en `IndiceSgm` qué número de entrada corresponde a cada segmento
(`ps`, `ks`, `cs`, `ds`, `es` y `ss`). Estos índices son necesarios porque, al omitir los segmentos
de tamaño cero, el número de entrada de cada tipo puede cambiar. Por ejemplo, si no hay Param ni
Const, Code queda en la entrada 0; si ambos existen, Code queda en la entrada 2.

Al terminar este paso, la MV tiene la memoria preparada y la tabla describe qué segmentos están
presentes, dónde comienzan y cuánto ocupan. La inicialización de registros y la ejecución del
programa ocurren después de esta parte del flujo.

## POR HACER: 