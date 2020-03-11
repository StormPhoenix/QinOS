extern  exception_handler
extern  hardware_interrupt_handler
extern  do_timer

[SECTION .text]

global  divide_error, debug, nmi, breakpoint, overflow, bounds
global  undefine_opcode, device_not_available, double_fault, coprocessor_segment_overrun
global  invalid_tss, segment_not_present, stack_error, general_protection
global  page_fault, reserved, coprocessor_error, alignment_check
global  timer_interrupt
global  irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8
global  irq9, irq10, irq11, irq12, irq13, irq14, irq15


; 时钟中断
irq0:
    iretd


timer_interrupt:
    ; 保存寄存器
    push    ds
    push    es
    push    fs
    push    edx
    push    ecx
    push    ebx
    push    eax

    ; 让段寄存器指向内核
    mov     eax, 0x10
    mov     ds, eax
    mov     es, eax
    ; fs 指向程序数据段
    mov     eax, 0x17
    mov     fs, eax
    ; TODO 参考 linux 时钟调度程序，这里会有一个 jiffies 来统计程序运行时间片
    ; 这里暂时不写 jiffies

    ; 告知8259A结束该硬件中断
    mov     al, 0x20
    out     0x20, al

    ; 取出压入堆栈的CS，提取DPL
    mov     eax, [esp + 32]
    and     eax, 0x3

    push    eax
    call    do_timer
    add     esp, 4

    pop     eax
    pop     ebx
    pop     ecx
    pop     edx
    pop     fs
    pop     es
    pop     ds
    iretd


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
	; 有 error_code
	; push	0xffffffff
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