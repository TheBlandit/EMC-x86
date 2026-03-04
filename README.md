# EMC-x86

The EMC (Exeter Mathematics Certificate) is a set of projects completed at EMS (Exeter Mathematics School).

This project is my 3rd EMC where I researched the x86 architecture, I have worked on this project over the course of year 13.

There is example x86 assembly code which I wrote, it includes a very basic kernel.

There is also a PDF which contains most of the information I learnt about x86, compared to the intel manuals it is a simplification whilst trying to keep most of the important detail. It also contains things not in the manuals such as various Linux syscalls, and BIOS.

There is also a poster which contains an overview of the x86 architecture.

## Things in the PDF

- Summary of x86
- Registers (inc MSRS)
- Cache (coherency protocol, placement policy, misses, memory types (MTRRs, PAT))
- Segmentation
- Paging (inc TLB)
- Task Management (inc TSS and call gates)
- Various common instructions
- Interrupts (sources and handling)
- PIC
- Local PIC (inc timer and interprocessor interrupts)
- Syscalls
- Hello world program (32-bit and 64-bit versions, Linux)
- Difference between two numbers program (Linux)
- Cat (print file contents) program (Linux)
- Hello world - Prints to the VGA text mode buffer (Baremetal via QEMU)
- Kernel (I call it a kernel, which should offend real kernels), it can take keyboard input (via PIC) and display it to the screen via VGA text mode. It also runs in an identity-mapped 64-bit environment (Baremetal via QEMU)

## Things not added (which I planned/may add)

- Stack management
- Directives
- Macros
- Memory fences and orderings
- Hardware management/interaction
- Using the FPU and floating point arithmetic instructions
- Virtualisation
- Linking and using the GOT
- Compilation
- SIMD
- AT&T vs intel syntax
- Sections (.bss)

## Using this work / contributing

If for some reason you want to contribute/use this work, feel free to but do notify me if you do.

## Acknowledgements

Thank you to EMS for supporting this project, in particular:
- Dr Aeran Fleming
- Dr Ed Horncastle
- Michael Andrejczuk
