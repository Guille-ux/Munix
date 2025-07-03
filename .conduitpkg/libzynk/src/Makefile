# SPDX-FileCopyrightText: 2025-present Guille <guilleleiratemes@gmail.com>
#
# SPDX-License-Identifier: GPLv3

# --- Variables de configuración ---
CC = gcc
AR = ar  # El archivador para crear bibliotecas estáticas
ARFLAGS = rcs # r: inserta con reemplazo, c: crea archivo, s: crea índice (tabla de símbolos)

# CFLAGS:
#   -Wall: Habilita todas las advertencias
#   -Wextra: Advertencias adicionales.
#   -g: Genera información de depuración para gdb.
#   -O2: Nivel de optimización 2.
#   -std=c11: Utiliza el estándar C11.
#   -I./runtime: Añade el directorio 'runtime' a la ruta de búsqueda de includes.
CFLAGS = -m32 -Wall -Wextra -g -O2 -std=c11 -I./runtime

# Directorio de fuentes y objetos
SRCDIR_RUNTIME = runtime
# IMPORTANTE: Usamos $(strip) para eliminar cualquier espacio accidental de la variable.
# Asegúrate de que NO haya espacios después del paréntesis de cierre en esta línea.
OBJDIR = obj

# Biblioteca estática final de ZynkL
TARGET = libzynk.a

# --- Archivos fuente para libzynkl.a ---
# Usa la función 'wildcard' para encontrar todos los archivos .c en el directorio runtime.
RUNTIME_SRCS = $(wildcard $(SRCDIR_RUNTIME)/*.c)
SRCS = $(RUNTIME_SRCS)

# --- Archivos objeto ---
# Transforma runtime/foo.c a obj/runtime/foo.o
OBJS = $(patsubst $(SRCDIR_RUNTIME)/%.c, $(OBJDIR)/$(SRCDIR_RUNTIME)/%.o, $(SRCS))

# --- Dependencias de cabecera (.d files) ---
# Los archivos .d se generarán junto a sus respectivos .o en obj/runtime/
DEPS = $(patsubst $(SRCDIR_RUNTIME)/%.c, $(OBJDIR)/$(SRCDIR_RUNTIME)/%.d, $(SRCS))

# --- Depuración: Comprueba cómo se expande SRCS ---
$(info Debug: Archivos C detectados en $(SRCDIR_RUNTIME)/: $(SRCS))
$(info Debug: Archivos Objeto (OBJS) calculados: $(OBJS))


# --- Regla principal: construye la biblioteca estática ---
all: $(TARGET)

# Crear el directorio 'obj' si no existe.
# IMPORTANTE: Asegúrate de que esta línea NO tenga espacios delante.
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Crear la biblioteca estática
# Depende del directorio 'obj' (para asegurar que exista) y de todos los archivos objeto.
$(TARGET): $(OBJDIR) $(OBJS)
	@echo "Creando la librería estática $(TARGET)..."
	$(AR) $(ARFLAGS) $@ $(OBJS)
	@echo "Creación de la librería completada."

# Compilar archivos fuente en objetos
# IMPORTANTE: Asegúrate de que esta línea NO tenga espacios delante.
$(OBJDIR)/$(SRCDIR_RUNTIME)/%.o: $(SRCDIR_RUNTIME)/%.c
	@mkdir -p $(@D) # Crea el subdirectorio (ej. obj/runtime/) si no existe.
	@echo "Compilando $< a $@"
	$(CC) $(CFLAGS) -c $< -o $@ -MMD -MP

# --- Limpieza ---
clean:
	@echo "Limpiando archivos generados..."
	@rm -rf $(OBJDIR) $(TARGET)
	@echo "Limpieza completada."

# --- Recompilar desde cero ---
rebuild: clean all

# --- Incluir archivos de dependencia ---
-include $(DEPS)

.PHONY: all clean rebuild
