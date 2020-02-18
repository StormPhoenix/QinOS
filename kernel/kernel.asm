extern	copy_gdt
extern	gdt_pointer


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
	; gdt 指针赋值
	sgdt 	[gdt_pointer]
	; gdt
	call	copy_gdt
	; 重加载 gdt
	lgdt	[gdt_pointer]
	jmp		$

	; 此时代码开始在 kernel 里面执行，在开始之前我选要重新设置 gdt 和堆栈
    ; mov	ah, 0Fh				; 0000: 黑底    1111: 白字
	; mov	al, 'K'
	; mov	[gs:((80 * 1 + 39) * 2)], ax	; 屏幕第 1 行, 第 39 列。
	; jmp	$