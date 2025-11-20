# Sobre la Tabla de Descriptores de Interrupciones (IDT y las rutinas de interrupción, ISR)

La **IDT** es una estructura que usa el procesador en x86 para saber que ejecutar cuando recibe una **syscall**, una **interrupción del hardware** o un **Error**.

***Nota:** En esta documentación solo tratare la **IDT** de 32bits y un poco de la de 64bits*

Primero de todo echemos un vistazo a el registro que el procesador usa para localizar la **IDT**, el **IDTR**

El **IDTR** se compone de:
- limite de 16 bits
- puntero de 32bits/64bits

Estructura de la **IDT**, la **IDT** es esencialmente un array de 256 entradas, ahora, veamos el código de **Munix** que se encarga de representar el array:
```c
typedef struct {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero; // always 0
	uint8_t type_attr; // gate type, dpl, and 'p'
	uint16_t offset_high;
} __attribute__((packed)) IDT32_Entry;

typedef struct {
	uint16_t offset_1;
	uint16_t selector;
	uint8_t ist; // 0 to 2 Interrupt Stack Tabke offset
	uint8_t type_attr;
	uint16_t offset_2;
	uint32_t offset_3;
	uint32_t zero;
} __attribute__((packed)) IDT64_Entry;
```

Primero hablemos de la estructura usada en 32bits, `IDT32_Entry`, esta estructura tiene
- `offset_low`: 16 bits bajos del puntero a la rutina
- `selector`: debe apuntar a un selector válido de tu `GDT`
- `zero`: siempre 0
- `type_attr`: tiene `gate type`, `dpl` y `p`
  	- `gate type`: valor de 4bits que representa 1 de 5 tipos
  	  	- `Task Gate`: `0x5`, el valor de offset debe ser 0
  	  	- `16b Interrupt Gate`: `0x6`
  	  	- `16b Trap Gate`: `0x7`
  	  	- `32b Interrupt Gate`: `0xE`
  	  	- `32b Trap Gate`: `0xF`
  	- `dpl`: 2 bits, indica nivel de privilegio.
  	- `p`:
- `offset_high`:
