#!/bin/bash

location=$(dirname $(realpath $0))
src="${location}/src"
link="${location}/link"
build="${location}/build"
boot="${build}/bootloader.bin"
image="${build}/image.bin"
nasm -f bin "${location}/bootloader.asm" -o "$boot" || exit 1
dd if=/dev/zero "of=$image" bs=512 count=2880 || exit 1
dd "if=$boot" "of=$image" bs=512 count=1 conv=notrunc || exit 1

o32=()

while read -r line; do
    cfile="${src}/$line"
    ofile="${build}/$line"
    ofile="${ofile%.*}.o"
    o32+=("$ofile")
    gcc -fno-stack-protector -m32 -ffreestanding -nostdlib -nostartfiles -fno-ident -fno-asynchronous-unwind-tables -c "$cfile" -o "$ofile" || exit 1
	# ^ Disable stack protector so we can return from _start
done < "${link}/link32" || exit 1

output="${build}/kernel32.bin"
elf="${build}/kernel32.elf"
ld -m elf_i386 -T "${link}/ld32" --entry=_start "${o32[@]}" -o "$elf" || exit 1
objcopy -O binary -R .note.gnu.property -R .note.gnu.build-id "$elf" "$output"

if [ $(stat -c%s "$output") -gt 4096 ]; then # Change to whatever the number of sectors * 512 the bootloader is reading
    echo "${output} file is too large"
    exit 1
fi

dd "if=$output" "of=$image" bs=512 seek=1 conv=notrunc || exit 1

o64=()

while read -r line; do
    cfile="${src}/$line"
    ofile="${build}/$line"
    ofile="${ofile%.*}.o"
    o64+=("$ofile")
    gcc -m64 -mgeneral-regs-only -ffreestanding -nostdlib -nostartfiles -fno-ident -fno-asynchronous-unwind-tables -c "$cfile" -o "$ofile" || exit 1
done < "${link}/link64" || exit 1

output="${build}/kernel64.bin"
elf="${build}/kernel64.elf"
ld -m elf_x86_64 -T "${link}/ld64" --entry=_start "${o64[@]}" -o "$elf" || exit 1
objcopy -O binary -R .note.gnu.property -R .note.gnu.build-id "$elf" "$output"

if [ $(stat -c%s "$output") -gt 4096 ]; then # Change to whatever the number of sectors * 512 the bootloader is reading
    echo "${output} file is too large"
    exit 1
fi

dd "if=$output" "of=$image" bs=512 seek=9 conv=notrunc || exit 1

# Run it with 256MiB memory (-cpu qemu64,+smep)
if [[ "$1" != "--norun" ]]; then
    qemu-system-x86_64 -drive format=raw,file="$image" -m 256
    # qemu-system-x86_64 -device isa-serial -M pc -drive format=raw,file="$image" -m 256
fi
