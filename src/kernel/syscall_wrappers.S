.section .text
.global sys_print
sys_print:
    movq $1, %rax
    movq %rdi, %rdi
    syscall
    ret

.global sys_exit
sys_exit:
    movq $2, %rax
    syscall
    ret

.global sys_open
sys_open:
    movq $3, %rax
    movq %rdi, %rdi
    syscall
    ret

.global sys_write
sys_write:
    movq $5, %rax
    movq %rdi, %rdi
    movq %rsi, %rsi
    movq %rdx, %rdx
    syscall
    ret

.global sys_read
sys_read:
    movq $4, %rax
    movq %rdi, %rdi
    movq %rsi, %rsi
    movq %rdx, %rdx
    syscall
    ret

.global sys_close
sys_close:
    movq $6, %rax
    movq %rdi, %rdi
    syscall
    ret
