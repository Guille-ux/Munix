.text
.globl start
start:
jmp multiboot_entry
.align 4
multiboot_header:
.long 0x1BADB002
.long 0x00000003
.long -(0x1BADB002+0x00000003)
multiboot_entry:
movl $(stack + 0x4000), %esp
call NextKernel_Main
loop: hlt
jmp loop
.section ".bss"
.comm stack,0x4000
