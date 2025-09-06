#!/bin/bash

echo "1: Arch and programming env supporting 32 and 64-bit
2: Full Instruction set reference
3: Systems programming
4: Model specific registers"

read SELECTED

DIR_PATH=$(dirname $(realpath $0))

if [ $SELECTED -ne "1" ] && [ $SELECTED -ne "2" ] && [ $SELECTED -ne "3" ] && [ $SELECTED -ne "4" ]; then
	exit 1
fi

xdg-open "${DIR_PATH}/x86v${SELECTED}.pdf" &
