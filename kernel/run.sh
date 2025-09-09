#!/bin/bash

# Assembled/compiles the files specified in 'link' then links them
# Each line consists of a file type, then a colon then the filename
# The filetypes are:
# a = assembly
# o = object
# r = 16 bit c
# p = 32 bit c
# l = 64 bit c

location=$(dirname $(realpath $0))
build="${location}/build"
boot="${build}/bootloader.bin"
nasm -f bin "${location}/bootloader.asm" -o "$boot" || exit 1

objects=()

while read -r line; do
    type=$(echo "$line" | cut -d':' -f1)
    name=$(echo "$line" | cut -d':' -f2-)
    file="${location}/$name"
    file2="${build}/$name"
    object="${file2%.*}.o"

    case "$type" in
        o)
            # Uses file instead of object in the case the file did not end in .o
            objects+=($file) ;;
        # r)
        #     objects+=($object)
        #     gcc -m16 -ffreestanding -c "$file" -o "$object" || exit 1 ;;
        p)
            objects+=($object)
            gcc -m32 -ffreestanding -nostdlib -nostartfiles -c "$file" -o "$object" || exit 1 ;;
            #gcc -m32 -nostartfiles -ffreestanding -c "$file" -o "$object" || exit 1 ;;
        l)
            objects+=($object)
            gcc -m64 -nostartfiles -ffreestanding -c "$file" -o "$object" || exit 1 ;;
        *)
            echo "'$file' cannot be compiled"
            exit 1 ;;
    esac

done < "${location}/link" || exit 1

output="${build}/kernel.bin"
image="${build}/image.bin"
elf="${build}/kernel.elf"
# ld -T "${location}/linker.ld" -o "$output" "${objects[@]}" || exit 1
ld -m elf_i386 -Ttext=0x1000 --entry=_start "${objects[@]}" -o "$elf" || exit 1
objcopy -O binary "$elf" "$output"
#ld -m elf_i386 -Ttext 0x1000 --oformat binary -o "$output" "${objects[@]}" || exit 1
dd if=/dev/zero "of=$image" bs=512 count=2880 || exit 1
dd "if=$boot" "of=$image" bs=512 count=1 conv=notrunc || exit 1
dd "if=$output" "of=$image" bs=512 seek=1 conv=notrunc || exit 1
# Run it with 256MiB memory (-cpu qemu64,+smep)
#qemu-system-x86_64 -fda "$image" -drive format=raw,file="$image" -m 256
#qemu-system-x86_64 -fda "$image" -m 256
qemu-system-x86_64 -drive format=raw,file="$image" -m 256
