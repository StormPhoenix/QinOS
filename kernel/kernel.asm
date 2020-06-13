extern  kernel_main
extern  cursor_pos

SELECTOR_KERNEL_CODE 		equ 0x8

[SECTION .bss]
	; 2KB 的用户空间
stack:
	resb	2 * 1024	
stack_top:


[SECTION .text]

global	_start

; Loader 设置好 GDT 和 PageTable 之后，会导入 kernel 模块，并跳转到 _start 处
; 此时程序执行到 _start 时，是处于保护与分页模式下
_start:
	; 切换堆栈
	mov 	esp, stack_top
    mov     dword [cursor_pos], 0
	; 重置下 CS 寄存器
	jmp 	SELECTOR_KERNEL_CODE:restart
restart:
	jmp		kernel_main
	jmp 	$
