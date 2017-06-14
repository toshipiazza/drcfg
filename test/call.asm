; compile with nasm -felf64 foo.asm; ld foo.o -o foo.out
bits 64

section .data

test2_ind dq test2

section .start
  global _start
_start:
  call test1        ; should not trigger

test1:
  call [test2_ind]  ; should trigger (?)

test2:
  push test3        ; should trigger
  call [rsp]

test3:
  mov rax, test4    ; should trigger
  call rax

test4:
  mov rax, 60
  syscall
