#!/bin/bash

# Definición de colores
ROJO='\e[31m'
VERDE='\e[32m'
AMARILLO='\e[33m'
AZUL='\e[34m'
CIAN='\e[36m'
BLANCO='\e[37m'
NEGRILLA='\e[1m'
SUBRALLADO='\e[4m'
RESET='\e[0m' # Siempre resetear al final

# script para decir donde esta cada parte de la libreria
cd ..
lib_zynk=$(pwd)
cd src
src_zynk=$(pwd)

echo -e "${CIAN}${NEGRILLA}[--------LibZynk Files---------]${RESET}"
echo -e "${SUBRALLADO}${AZUL}libzynk.a${RESET}:${BLANCO}${NEGRILLA} Static Library ${RESET}- ${SUBRALLADO}${AMARILLO}$lib_zynk/libzynk.a${RESET}"
echo -e "${SUBRALLADO}${AZUL}zynk.h${RESET}:${BLANCO}${NEGRILLA} Headers ${RESET}- ${SUBRALLADO}${AMARILLO}$src_zynk/zynk.h${RESET}"
