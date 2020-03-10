extern  sys_call_table
extern  NR_syscalls

NR_syscall_bounds       equ -57
NR_syscall_undefine     equ -58

bits 32

[SECTION .text]
global  system_call

system_call:
    push    ds
    push    es
    push    fs

    ; edx ebx ecx 可能是系统调用的参数
    push    edx
    push    ecx
    push    ebx

    ; ds es 指向内核空间
    mov     ebx, 0x10
    mov     ds, ebx
    mov     es, ebx
    ; fs 程序数据段
    mov     ebx, 0x17
    mov     fs, ebx
    ; 系统调用号超过范围
    cmp     eax, NR_syscalls
    ja      bad_sys_call

    ; 判断系统调用函数是否存在
    mov     dword ebx, [sys_call_table + eax * 4]
    cmp     ebx, 0
    je      default_sys_call

    call    [sys_call_table + eax * 4]
    ; call    ebx
    push    eax
    jmp     ret_from_sys_call

bad_sys_call:
    push    NR_syscall_bounds
    jmp     ret_from_sys_call

default_sys_call:
    push    NR_syscall_undefine
    jmp     ret_from_sys_call

ret_from_sys_call:
    pop     eax
    pop     ebx
    pop     ecx
    pop     edx
    pop     fs
    pop     es
    pop     ds
    iret