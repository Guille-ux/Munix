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
	qemu-system-i386 -cdrom munix.iso \
			-drive file=disk.img,format=raw,if=ide \
			-boot d \
			-m 128M \
			-serial stdio \
			-no-reboot \
			-s
	echo "[ Máquina Virtual Finalizada!   ]"
	exit
fi
if [ "$1" == "liball" ]
then
	./debug.sh update
	./debug.sh build-libs
	rm -rf Munix/libs
	mkdir -p Munix/libs
	cp -r libs/* Munix/libs
	cp -r ../.conduitpkg/libcs2 Munix/libs
	exit
fi
if [ "$1" == "all" ]
then
	./debug.sh compile
	./debug.sh run
	exit
fi
if [ "$1" == "-a" ]
then
	./debug.sh liball
	./debug.sh all
	exit
fi

echo "[ Comando No Reconocido ]"
