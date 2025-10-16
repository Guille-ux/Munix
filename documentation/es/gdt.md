# Sobre la GDT (Global Descriptor Table)

**GDT** es una estructura de datos extremadamente importante en sistemas de 32bits, y aunque principalmente reemplazada por la paginación en sistemas de 64 bits sigue siendo necesaria
para entrar al modo protegido.

La **GDT** es esencialmente una tabla de entradas llamadas descriptores de segmento, el registro GDTR almacena la dirección base de esta y su limite (tamaño).

Cada descriptor de segmento (un `GDT_Entry` en el código de Munix lineas 7-14 de include/gdt.h)
tendría más o menos lo siguiente:

- **Un Límite** de 20 bits (tamaño de la región)
- **Una Base** de 32 bits
- **Flags de Acceso**: 8 bits
- **Granularity**: 4 bits. esto es raro, pero básicamente es un campo de 8 bits cuyos bits 0, 1, 2, 3 son los últimos bits del Límite, el 4 esta reservado y los últimos 3 son para otras cosas.

**Organización** de los datos.
***Nota**: Estan puestos por orden.*
- `limit_low` → primeros 16 bits del límite
- `base_low` → primeros 16 bits de la base
- `base_middle` → siguientes 8 bits de la base
- `access` → esta compuesto de los siguientes bits que representan lo siguiente
	- bit 0: **Access Bit**, la CPU lo configura a 1 cuando se accede al segmento por primera vez.
   	- bit 1: **RW**, si es un segmento de código 0 significa que no se puede leer y 1 lo contrario, para segmentos de datos 1 significa que se puede escribir y 0 que no se puede escribir.
   	- bit 2: **DC**
   	  	- Para Selectores de Datos: 0 significa que el segmento crece hacía arriba y 1 que crece hacía abajo.
   	  	- Para Selectores de Código: 0 significa que solo puede ser ejecutado por el anillo de privilegios **DPL** (0==mayor privilegio y 3==menor privilegio), en cambio 1 significa que el código puede ser ejecutado por cualquier nivel de privilegios igual o menor.
  	- bit 3: **E**: un 0 signifíca que es un segmento de datos y un 1 signifíca que es un segmento de código.
  	- bit 4: **S**: un 0 signifíca que es un segmento de sistema (por ejemplo, *Task State Segment*) y un 1 significa que es o un segmento de datos o de código.
  	- bits 5 y 6. **DPL**: contiene el anillo de privilegios del segmento, es decir, los permisos que tiene el código ejecutado en este segmento.
  	- bit: 7: **P**: Bit de Presencia, indica si es un segmento válido, debe ser 1 para cualquier segmento válido.
- `granularity`
  - Bits 0-3: parte restante del límite
  - Bit 4: Son cosas internas del hardware, **NO TOCAR**
  - Bit 5: **L**, si es un segmento de código, indica si es de 64 bits.
  - Bit 6: **DB**, si el bit es 1 es un segmento de 32 bits en caso contrario sera de 16 bits (o 64 si L es 1).
  - Bit 7: **Granularity**, si es 1 el tamaño del segmento va por páginas de 4096 kb, si es 0, entonces definira el tamaó en bytes.

```c
typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed)) GDT_Entry;
```

**GDT_Pointer**: es una estructura encargada de apuntar donde esta la tabla **GDT** en la memoria RAM, sería algo como lo siguiente:
```c
typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) GDT_Pointer;
```

Rellenar la **GDT** es muy sencillo, el código de **Munix** usa la función `void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);` (función que se encuentra en `src/gdt.c` y `include/gdt.h`), esta función es usada sobre un array estático `gdt_table` del tipo `GDT_Entry` y es únicamente usada desde `gdt_init()` con el objetivo de rellenar la tabla.

Descriptores de Segmento **OBLIGATORIOS** (para que el kernel funcione):
- **Null Descriptor** (peculiaridad de x86), añades una entrada con todo a 0 (debe de ser la primera entrada).
- **Kernel Code Descriptor**: Describe el segmento de código del kernel, deberia apuntar a toda la memoria que se pueda, para poder usar toda la memoria RAM.
 - Flags de Granularidad:
   - **Granularity**: 1 (páginas de 4096 bytes).
   - **DB**: 1 (segmento de 32 bits).
   - **L**: 0 (en 32 bits).
 - Flags de Acceso:
   - **P**: 1 (activado).
   - **DPL**: 00 (ring 0).
   - **S**: 0 (segmento de código/datos).
   - **E**: 1 (segmento de código).
   - **DC**: 0 (solo se puede ejecutar por ring 0).
   - **RW**: 1 (se puede leer el contenido).
   - **A**: 0 (esto siempre se configura a 0).
- **Kernel Data Descriptor**: Describe el segmento de datos del kernel, deberia al igual que el anterior apuntar a toda la memoria.
 - Flags de Granularidad (son los mismo que en código).
 - Flags de Acceso:
   - **P**: 1.
   - **DPL**: 00.
   - **S**: 1.
   - **E**: 0.
   - **DC**: 0.
   - **RW**: 1.
   - **A**: 0 (siempre se configura a 0).
  
Y ahora, a cargar la **GDT**!

Para cargar la **GDT** primero debemos de comprender los **registros de segmento**, son 6:
- **CS**: Registro de Segmento de Código (donde se leen las instrucciones).
- **SS**: Registro de Segmento de Pila (las operaciones de pila operan dentro de el).
- **DS**: Registro de Segmento de Datos, contiene el selector del segmento de datos predeterminado para acceder a datos globales o locales.
- **ES**, **GS**, **FS**: registros de segmento extra.

Los **registros de segmento** son de 16 bytes, e indican el indice (en bytes) del segmento en la **GDT**, por ejemplo si el segmento de código del kernel esta en el indice 0, entonces el registro **CS** tendra `0x08` (las entradas son de 8 bytes cada 1).ç

Cargando la **GDT**...

paso nº 1, cargar el gdt_pointer con

```asm
lgdt [gdt_ptr]
```

luego de esto ya tendremos nuestra tabla cargada, pero ahora tenemos que actualizar los registros.

para actualizar **CS** realizaremos un salto de segmento:

```asm
jmp KERNEL_CS:flush_cs
```

siendo `KERNEL_CS` el indice del segmento de código del kernel en la **GDT** (multiplicado por 8) y flush_cs una parte del código donde trabajaremos con los demás **registros de segmento**.

ahora, actualizaremos todos los demás registros para que apunten a `KERNEL_DS` (segmento de datos del kernel):

```asm
flush_cs:
	mov ax, KERNEL_DS
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret
```

al final de la función usamos ret para volver a antes de cargar la **GDT**.
