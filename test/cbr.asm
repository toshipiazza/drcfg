bits 64

section .start
  global _start
_start:
  xor rax, rax
  jz always
  mov rax, [rax] ; crash
always:
  add rax, 1
  jz never
  jmp should_not_trigger
should_not_trigger:
  mov rax, 60
  syscall
never:
  mov rax, [rax] ; crash
