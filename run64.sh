#!/bin/bash

if [ $# -eq 0 ]; then
	echo "Incorrect Args"
	exit 1
fi

OBJECTS=()

ASM="$(pwd)/$1"
if [[ $ASM == *.asm ]]; then
	BASE="${ASM%.asm}"
	OBJECT="${BASE}.o"
	OBJECTS+=($OBJECT)
	nasm -f elf64 $ASM -o $OBJECT || exit 1
else
	echo "File does not end in .asm"
	exit 1
fi

for arg in "${@:2}"; do
	ASM="$(pwd)/$arg"
	if [[ $ASM == *.asm ]]; then
		OBJECT="${ASM%.asm}.o"
		OBJECTS+=($OBJECT)
		nasm -f elf64 $ASM -o $OBJECT || exit 1
	else
		echo "File does not end in .asm"
		exit 1
	fi
done

ld "${OBJECTS[@]}" -o $BASE || exit 1
$BASE

