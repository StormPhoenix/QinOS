extern  kernel_main

SELECTOR_KERNEL_CODE 		equ 0x8

[SECTION .bss]
	; 2KB 的用户空间
stack:
	resb	2 * 1024	
stack_top:


[SECTION .text]

global	_start


_start:
	; 切换堆栈
	mov 	esp, stack_top
	jmp 	SELECTOR_KERNEL_CODE:restart
restart:
	jmp		kernel_main
	jmp 	$
