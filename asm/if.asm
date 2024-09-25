BITS 64
pop rax
pop rbx
pop rcx
cmp rax, 0
jg __ok
push rcx
jmp __end
__ok:
push rbx
__end: