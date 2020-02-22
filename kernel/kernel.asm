extern	setup_gdt
extern  init_idt
extern	gdt_ptr
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
global  divide_error, debug, nmi, breakpoint, overflow, bounds
global  undefine_opcode, device_not_available, double_fault, coprocessor_segment_overrun
global  invalid_tss, segment_not_present, stack_error, general_protection
global page_fault, reserved, coprocessor_error, alignment_check
global  irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8
global  irq9, irq10, irq11, irq12, irq13, irq14, irq15


_start:
	; 切换堆栈
	mov 	esp, stack_top
	; gdt 指针赋值
	sgdt 	[gdt_ptr]
	; gdt
	call	setup_gdt
	; 重加载 gdt
	lgdt	[gdt_ptr]
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


debug:
	; 没有 error_code
	push	0xffffffff
	push	1
	jmp		exception


nmi:
	; 没有 error_code
	push	0xffffffff
	push	2
	jmp		exception


breakpoint:
	; 没有 error_code
	push	0xffffffff
	push	3
	jmp		exception


overflow:
	; 没有 error_code
	push	0xffffffff
	push	4
	jmp		exception


bounds:
	; 没有 error_code
	push	0xffffffff
	push	5
	jmp		exception


undefine_opcode:
	; 没有 error_code
	push	0xffffffff
	push	6
	jmp		exception


device_not_available:
	; 没有 error_code
	push	0xffffffff
	push	7
	jmp		exception


double_fault:
	; 没有 error_code
	push	0xffffffff
	push	8
	jmp		exception


coprocessor_segment_overrun:
	; 没有 error_code
	push	0xffffffff
	push	9
	jmp		exception


invalid_tss:
	; 没有 error_code
	push	0xffffffff
	push	10
	jmp		exception


segment_not_present:
	; 没有 error_code
	push	0xffffffff
	push	11
	jmp		exception


stack_error:
	; 没有 error_code
	push	0xffffffff
	push	12
	jmp		exception


general_protection:
	; 没有 error_code
	push	0xffffffff
	push	13
	jmp		exception


page_fault:
	; 没有 error_code
	push	0xffffffff
	push	14
	jmp		exception


reserved:
	; 没有 error_code
	push	0xffffffff
	push	15
	jmp		exception


coprocessor_error:
	; 没有 error_code
	push	0xffffffff
	push	16
	jmp		exception


alignment_check:
	push	0xffffffff
	push 	17
	jmp 	exception


exception:
	call	exception_handler
	add		esp, 8
	hlt