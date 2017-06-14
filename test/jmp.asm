; compile with nasm -felf64 foo.asm; ld foo.o -o foo.out
bits 64

section .start
  global _start
_start:
  jmp static_label

static_label:
  mov rax, dynamic_label
  jmp rax

dynamic_label:

  ; exit
  mov rax, 60
  syscall

