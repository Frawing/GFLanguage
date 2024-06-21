section .text

default rel
global _start
extern _exit, strlen


            print_number:
                mov rcx, printNumberBuffer
                mov rbx, 0
                mov [rcx], rbx
                inc rcx
                mov [printNumberBufferPos], rcx
            print_number_loop:
                mov rdx, 0
                mov rbx, 10
                div rbx 
                push rax
                add rdx, 48
                mov rcx, [printNumberBufferPos]
                mov [rcx], dl
                inc rcx
                mov [printNumberBufferPos], rcx
                pop rax
                cmp rax, 0
                jne print_number_loop
            print_number_loop_2:
                mov rcx, [printNumberBufferPos]
                mov rax, 1
                mov rdi, 1
                mov rsi, rcx
                mov rdx, 1
                syscall
                mov rcx, [printNumberBufferPos]
                dec rcx
                mov [printNumberBufferPos], rcx
                cmp rcx, printNumberBuffer
                jge print_number_loop_2
                ret

        
_start:
    mov rdi, label1
    call strlen
    mov rsi, rdi
    mov rdx, rax
    mov rdi, 1
    mov rax, 1
    syscall
    push 12414
    pop ax
    call print_number
    mov rdi, label2
    call strlen
    mov rsi, rdi
    mov rdx, rax
    mov rdi, 1
    mov rax, 1
    syscall
    push 493
    mov rdi, label3
    push rdi
    mov rax, 83
    pop rdi
    pop rsi
    syscall
    test rax, rax
    js exit_with_error
    push 420
    push 577
    mov rdi, label4
    push rdi
    mov rax, 2
    pop rdi
    pop rsi
    pop rdx
    syscall
    test rax, rax
    js exit_with_error
    xor rdi, rdi
    call _exit

exit_with_error:
    mov rax, 60
    mov rdi, 1
    syscall
section .data

GF_TRUE : db 255
GF_FALSE : db 0
label1 : db `Ciao user`, 0
label2 : db `!!\n`, 0
label3 : db `Ciao_sono_una_cartella`, 0
label4 : db `Ciao_sono_una_cartella/TEST.txt`, 0

section .bss

printNumberBuffer resb 25
printNumberBufferPos resb 8
