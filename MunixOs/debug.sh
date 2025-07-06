#!/bin/bash

echo "[ Script para la construcción de Munix ]"

if [ "$1" == "update" ]
then
	echo "[ Actualizando... ]"
	../upcpkg libcs2
	echo "[ Actualizado!    ]"
	exit
fi
if [ "$1" == "compile" ]
then
	echo "[ Compilando... ]"
	./compile.sh
	echo "[ Compilado!    ]"
	exit
fi
if [ "$1" == "build-libs" ]
then
	echo "[ Construyendo Librerias... ]"
	cd libs
	./build.sh
	cd ..
	echo "[ Librerias Construidas!    ]"
	exit
fi
if [ "$1" == "run" ]
then
	echo "[ Ejecutando Máquina Virtual... ]"
	qemu-system-i386 munix.iso
	echo "[ Máquina Virtual Finalizada!   ]"
	exit
fi

echo "[ Comando No Reconocido ]"
