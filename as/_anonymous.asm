section .data
reste: dq 0
difference: dq 0
uppercase: dq 0
letter: dq 0
produit: dq 0
digit: dq 0
punct: dq 0
somme: dq 0
quotient: dq 0
section .text
global _start
_start:
push 12
pop qword [somme]
push 24
pop qword [produit]
push 'c'
pop qword [letter]
push '0'
pop qword [digit]
push 24
push 12
;divstar
mov rdx, 0
pop rcx
pop rax
idiv rcx
push rax
pop qword [quotient]
push qword [letter]
push 'A'
push 'a'
;addsub
pop rcx
pop rax
sub rax, rcx
push rax
;addsub
pop rcx
pop rax
add rax, rcx
push rax
pop qword [uppercase]
push qword [somme]
push qword [produit]
; order
pop rax
pop rcx
cmp rax, rcx
jl else0
push 1
pop qword [somme]
jmp endif0
else0:
push qword [somme]
push qword [produit]
; order
pop rax
pop rcx
cmp rax, rcx
jg endif0
push 2
pop qword [somme]
endif0:
push qword [somme]
push qword [produit]
; order
pop rax
pop rcx
cmp rax, rcx
jl endif1
push qword [produit]
pop qword [somme]
endif1:
;finish
mov rax, 60
mov rdi, 0
syscall
