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

o32=()

while read -r line; do
    cfile="${location}/$line"
    ofile="${build}/$line"
    ofile="${ofile%.*}.o"
    o32+=($ofile)
    gcc -m32 -ffreestanding -nostdlib -nostartfiles -fno-ident -fno-asynchronous-unwind-tables -c "$cfile" -o "$ofile" || exit 1
done < "${location}/link32" || exit 1

output="${build}/kernel.bin"
image="${build}/image.bin"
elf="${build}/kernel.elf"
ld -m elf_i386 -T "${location}/ld" --entry=_start "${o32[@]}" -o "$elf" || exit 1
#ld -m elf_i386 -Ttext=0x1000 --entry=_start "${objects[@]}" -o "$elf" || exit 1
objcopy -O binary -R .note.gnu.property -R .note.gnu.build-id "$elf" "$output"
if [[ $(stat -c%s "$output") > 512 ]]; then # Change to whatever the number of sectors * 512 the bootloader is reading
    echo "${output} file is too large"
    exit 1
fi
dd if=/dev/zero "of=$image" bs=512 count=2880 || exit 1
dd "if=$boot" "of=$image" bs=512 count=1 conv=notrunc || exit 1
dd "if=$output" "of=$image" bs=512 seek=1 conv=notrunc || exit 1
# Run it with 256MiB memory (-cpu qemu64,+smep)
if [[ "$1" != "--norun" ]]; then
    qemu-system-x86_64 -drive format=raw,file="$image" -m 256
fi
