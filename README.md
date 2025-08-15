# EMC-x86

##
Useful Links
https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/
https://github.com/torvalds/linux/tree/master/arch/x86/entry/syscalls

## Objectives

Key:
Sign | Completeness
- | -
\-   | Not attempted
\+   | Partially complete
\=   | Complete

### Primary objectives

Commonly used instructions of the x86 instruction set (16, 32 and 64bit varients)
Using Linux syscalls (when running on linux)
Interacts with the BIOS (when running baremetal/virtualised, and using vga text mode to display text)
Functions calls
Stack (and heap) memory management
A bootloader that changes from real to protected to long mode 
x86 Directives

### Secondary objectives / potential extentions

Atomic instructions, mutexes/spinlocks, memory fences and orderings
Hardware management/interaction
Writing a basic kernel that has a simple terminal. This can be expanded to run user-mode programs which use syscalls to communicate with the kernel, furthermore Rust code could be used to write the programs which interact with the assembly kernel 
Virtual memory - and investigating methods to reduce memory segmentation
Using the FPU and floating point arithmetic instructions
Virtualisation
Linking and using the GOT
File Management (on linux)
Compilation
SIMD

#### Things to add to research document
Variable alignment and size
Assembler, linker, object files, binaries, GOT
AT&T vs intel
Boolean operations / usage
Segment registers
Machine code

<details>
  <summary>

  ## Click here to view the document

  </summary>
 
  <embed src="./latex/main.pdf" type="application/pdf" width="100%" height="600px">
 
 </details>
