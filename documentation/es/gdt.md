# Sobre la GDT (Global Descriptor Table)

**GDT** es una estructura de datos extremadamente importante en sistemas de 32bits, y aunque principalmente reemplazada por la paginación en sistemas de 64 bits sigue siendo necesaria
para entrar al modo protegido.

La **GDT** es esencialmente una tabla de entradas llamadas descriptores de segmento, el registro GDTR almacena la dirección base de esta y su limite (tamaño).

Cada descriptor de segmento (un `GDT_Entry` en el código de Munix lineas 7-14 de include/gdt.h)
tendría más o menos lo siguiente:

- **Un Límite** de 20 bits (tamaño de la región)
- **Una Base** de 32 bits
- **Flags de Acceso**: 8 bits
- **Granularity**: 4 bits. esto es raro, pero básicamente es un campo de 8 bits cuyos bits 0, 1, 2, 3 son los últimos bits del Límite, los otros 2 se refieren a tamaño de páginas etc

**Organización** de los datos.
***Nota**: Estan puestos por orden.*
- `limit_low` → primeros 16 bits del límite
- `base_low` → primeros 16 bits de la base
- `base_middle` → siguientes 8 bits de la base
- `access` → esta compuesto de los siguientes bits que representan lo siguiente
	- bit 0: **Access Bit**, la CPU lo configura a 1 cuando se accede al segmento.
   	- bit 1: **RW**, si es un segmento de código 0 significa que no se puede leer y 1 lo contrario, para segmentos de datos 1 significa que se puede escribir y 0 que no se puede escribir.
   	- bit 2: **DC**
   	  	- Para Selectores de Datos: 0 significa que el segmento crece hacía arriba y 1 que crece hacía abajo.
   	  	- Para Selectores de Código: 0 significa que solo puede ser ejecutado por el anillo de privilegios **DPL** (0==mayor privilegio y 3==menor privilegio), en cambio 1 significa que el código puede ser ejecutado por cualquier nivel de privilegios igual o menor.
  	- bit 3: **E**: un 0 signifíca que es un segmento de datos y un 1 signifíca que es un segmento de código.
  	- bit 4: **S**: un 0 signifíca que es un segmento de sistema (por ejemplo, *Task State Segment*) y un 1 significa que es o un segmento de datos o de código.
  	- bits 5 y 6. **DPL**: contiene el anillo de privilegios del segmento, es decir, los permisos que tiene el código ejecutado en este segmento.
  	- bit: 7: **P**: Bit de Presencia, indica si es un segmento válido, debe ser 1 para cualquier segmento válido.

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

`limit_low` y `base_low`, son la parte baja de `base` y `limit` respectivamente.
