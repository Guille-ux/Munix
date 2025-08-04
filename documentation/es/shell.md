# Documentación Técnica de mbash
[volver atras](doc-index.md)

## Introducción

mbash es un intérprete de comandos minimalista diseñado originalmente para un sistema operativo personalizado.

## Arquitectura del Sistema

mbash sigue una arquitectura clásica de tres etapas:

1. **Lexer**: Convierte el texto de entrada en tokens
2. **Parser**: Transforma los tokens en un Árbol de Sintaxis Abstracta (AST)
3. **Evaluador**: Ejecuta el AST y produce resultados

### Diagrama de Flujo

```
Texto de entrada → [Lexer] → Tokens → [Parser] → AST → [Evaluador] → Resultados
```

## Componentes Detallados

### 1. Sistema de Tipos (mbtype)

**Propósito**: Proporcionar funciones básicas de verificación de tipos de caracteres.

**Funciones clave**:
- `k_isspace()`: Verifica si un carácter es espacio en blanco
- `k_isalpha()`: Verifica si es letra
- `k_isdigit()`: Verifica si es dígito
- `k_isalnum()`: Verifica si es alfanumérico

### 2. Analizador Léxico (lexer)

**Tokens soportados**:
- Operadores matemáticos: `+`, `-`, `*`, `/`
- Operadores de comparación: `==`, `!`, `<`, `>`
- Símbolos especiales: `(`, `)`, `{`, `}`, `;`, `:`, `$`
- Palabras clave: `if`, `then`, `else`, `fi`, `while`, `do`, `for`, `done`, `break`, `return`, `export`, `echo`
- Literales: números, strings entre comillas

**Ejemplo de tokenización**:
```bash
if $1:"var" == 42 then
  echo "hola"
fi
```
Se convierte en:
1. `TOKEN_IF`
2. `TOKEN_DOLLAR`
3. `TOKEN_NUMBER` (1)
4. `TOKEN_COLON`
5. `TOKEN_STRING` ("var")
6. `TOKEN_EQ`
7. `TOKEN_NUMBER` (42)
8. `TOKEN_THEN`
9. `TOKEN_ECHO`
10. `TOKEN_STRING` (hola)

### 3. Analizador Sintáctico (parser)

**Estructura del AST**:

El parser construye un árbol donde cada nodo puede ser:

- **Declaraciones**:
  - Asignaciones (`NODE_ASSIGNMENT`)
  - Llamadas a comandos (`NODE_COMMAND_CALL`)
  - Estructuras de control (`NODE_IF`, `NODE_WHILE_LOOP`, `NODE_FOR_LOOP`)
  - Exportaciones (`NODE_EXPORT`)

- **Expresiones**:
  - Literales (`NODE_NUM_LIT`, `NODE_STRING_LIT`)
  - Referencias (`NODE_VAR_REF`, `NODE_ARG_REF`)
  - Operaciones (`NODE_BINARY_EXPR`, `NODE_UNARY`)

**Ejemplo de parsing**:

Para el código:
```bash
1:"var" = 10 + 20
```

El AST generado sería:
```
NODE_ASSIGNMENT
├── num: 1
├── var_name: "var"
└── expr: NODE_BINARY_EXPR
    ├── op: TOKEN_PLUS
    ├── left: NODE_NUM_LIT (10)
    └── right: NODE_NUM_LIT (20)
```

### 4. Evaluador (eval)

**Sistema de valores**:
- `VAL_NUMBER`: Almacena un entero (long)
- `VAL_STRING`: Almacena una cadena (char*)
- `VAL_NONE`: Valor especial para representar ausencia de valor

**Contexto de ejecución**:
El evaluador mantiene un estado de ejecución que incluye:
- Tabla de variables
- Argumentos disponibles
- Flags de control (break, return, exit)
- Código de salida

**Ejemplo de evaluación**:

Para la expresión `$1:"var" + 5`:
1. Obtiene el valor de la variable 1:"var"
2. Evalúa la suma con el literal 5
3. Devuelve el resultado

## Sistema de Variables

### Sintaxis
`$[número]:[nombre]`

Ejemplo: `$1:"mi_var"`

### Características:
- Las variables se referencian por ID numérico y nombre
- El ID numérico permite un acceso rápido en la tabla de variables
- El nombre es opcional pero útil para depuración

### Asignación:
```bash
1:"mi_var" = 42      # Asignación directa
2:"otra" = $1:"mi_var"  # Copia de valor
```

## Estructuras de Control

### 1. Condicionales (if)

**Sintaxis**:
```bash
if condición then
    # código
[else
    # código]
fi
```

**Ejemplo**:
```bash
if $1:"var" == 42 then
    echo "Es 42"
else
    echo "No es 42"
fi
```

### 2. Bucles (while)

**Sintaxis**:
```mbash
do {
    # código
} while condición
```

**Ejemplo**:
```mbash
do {
    echo "Hola"
    1:"i" = $1:"i" + 1
} while $1:"i" < 5
```

### 3. Bucles (for)

**Sintaxis**:
```bash
for (inicialización; condición; incremento) do
    # código
done
```

**Ejemplo**:
```bash
for (1:"i" = 0; $1:"i" < 10; $1:"i" = $1:"i" + 1) do
    echo $1:"i"
done
```

## Comandos Especiales

### 1. echo
Imprime argumentos a la salida estándar.

```bash
echo "Hola mundo"
echo 42
echo $1:"mi_var"
```

### 2. export
Marca una variable para exportación (implementación básica actual).

```bash
export $1:"mi_var"
```

### 3. break
Interrumpe la ejecución de un bucle.

```mbash
do {
    echo "Infinito"
    break
} while 1
```

### 4. return
Finaliza la ejecución con un valor de retorno.

```mbash
return 0
return $1:"resultado"
```

## Ejemplos Completos

### 1. Factorial

```mbash
# Calcula factorial de 5
1:"n" = 5
2:"result" = 1

for (1:"i" = 1; $1:"i" < $1:n+1 ; 1:"i" = $1:"i" + 1) do
    2:"result" = $2:"result" * $1:"i"
done

echo "Factorial de" $1:"n" + "es" + $2:"result"
```

### 2. Fibonacci

```mbash
# Muestra los primeros 10 números de Fibonacci
1:"a" = 0
2:"b" = 1
3:"count" = 0

do {
    echo $1:"a"
    3:"temp" = $1:"a"
    1:"a" = $2:"b"
    2:"b" = $3:"temp" + $2:"b"
    3:"count" = $3:"count" + 1
} while $3:"count" < 10
```

## Consideraciones de Implementación

### 1. Gestión de Memoria
- Uso de `kmalloc`/`kfree` en lugar de malloc/free estándar
- Conteo de referencias para strings
- Liberación recursiva de estructuras AST

### 2. Manejo de Errores
- Códigos de retorno claros (EVAL_OK, EVAL_ERR, etc.)
- Mensajes de error descriptivos
- Verificación de nulos en operaciones críticas

### 3. Rendimiento
- Tablas de variables con crecimiento dinámico
- Evaluación perezosa de expresiones
- Minimización de copias de datos

## Limitaciones Actuales

1. **Pipes**: No implementados (planeados para futuras versiones)
2. **Redirecciones**: No soportadas
3. **Funciones**: No hay soporte para definición de funciones
4. **Arrays**: Solo variables escalares
5. **Scripting**: Capacidades limitadas de scripting

## Futuras Mejoras

1. Implementación de pipes y redirecciones
2. Soporte para funciones y scoping
3. Mejor manejo de strings y arrays
4. Comandos built-in adicionales
5. Sistema de módulos/extensión
