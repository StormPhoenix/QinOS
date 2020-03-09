extern	setup_gdt
extern  setup_idt
extern	gdt_ptr
extern  idt_ptr
extern  load_ldt
extern  load_task
extern  kernel_main

SELECTOR_KERNEL_CODE 		equ 0x8

[SECTION .bss]
	; 2KB 的用户空间
stack:
	resb	2 * 1024	
stack_top:


[SECTION .text]

global	_start
global  move_to_user_mode


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
	call	setup_idt
	lidt	[idt_ptr]

	jmp 	SELECTOR_KERNEL_CODE:restart
restart:
	sti
	jmp		kernel_main
	jmp 	$


; 跳转到用户态
move_to_user_mode:
    mov     eax, esp
	; ss 入栈
	push    0x17
	; esp 入栈
	push    eax
	; eflags 入栈
	pushf
	; cs 入栈
	push    0x0f
	; eip 入栈
	push    .1
	iret
.1:
    mov     eax, 0x17
    mov     ds, eax
    mov     es, eax
    mov     fs, eax
    mov     eax, 0x1b
    mov     gs, eax
    ret

; 加载 tss
load_task:
    mov     eax, [esp + 4]
    ltr     ax
    ret


; 加载 ldt
load_ldt:
     mov     eax, [esp + 4]
     lldt    ax
     ret

