extern	setup_gdt
extern  init_idt
extern	gdt_pointer
extern  exception_handler
extern  hardware_interrupt_handler
extern  idt_ptr

SELECTOR_KERNEL_CODE 		equ 0x8

[SECTION .bss]
	; 2KB 的用户空间
stack:
	resb	2 * 1024	
stack_top:


[SECTION .text]

global	_start
global  divide_error, undefine_opcode
global  irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8
global  irq9, irq10, irq11, irq12, irq13, irq14, irq15

_start:
	; 切换堆栈
	mov 	esp, stack_top
	; gdt 指针赋值
	sgdt 	[gdt_pointer]
	; gdt
	call	setup_gdt
	; 重加载 gdt
	lgdt	[gdt_pointer]
	; 设置 idt
	call	init_idt
	lidt	[idt_ptr]

	jmp 	SELECTOR_KERNEL_CODE:restart
restart:
	ud2
	jmp		$


irq0:
	push 	0
	jmp 	hardware_interrupt


irq1:
	push 	1
	jmp 	hardware_interrupt


irq2:
	push 	2
	jmp 	hardware_interrupt


irq3:
	push 	3
	jmp 	hardware_interrupt


irq4:
	push 	4
	jmp 	hardware_interrupt


irq5:
	push 	5
	jmp 	hardware_interrupt


irq6:
	push 	6
	jmp 	hardware_interrupt


irq7:
	push 	7
	jmp 	hardware_interrupt


irq8:
	push 	8
	jmp 	hardware_interrupt


irq9:
	push 	9
	jmp 	hardware_interrupt


irq10:
	push 	10
	jmp 	hardware_interrupt


irq11:
	push 	11
	jmp 	hardware_interrupt


irq12:
	push 	12
	jmp 	hardware_interrupt


irq13:
	push 	13
	jmp 	hardware_interrupt


irq14:
	push 	14
	jmp 	hardware_interrupt


irq15:
	push 	15
	jmp 	hardware_interrupt


hardware_interrupt:
	call 	hardware_interrupt_handler
	add		esp, 4
	hlt


divide_error:
	; 没有 error_code
	push	0xffffffff
	push	0
	jmp		exception


undefine_opcode:
	push	0xffffffff
	push 	6
	jmp 	exception


exception:
	call	exception_handler
	add		esp, 8
	hlt