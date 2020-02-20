[SECTION  .data]
; 屏幕光标位置，初始位置在 [0, 0]
POINTER_POS:
    dd      (80 * 17 + 0) * 2

[SECTION .text]

global  copy_memory
global  print_string

; ------------------------------------------------
; copy_memory
; 参数：
;   src     源位置，[esp + 4]
;   size    复制字节数量 [esp + 8]
;   dst     目标位置 [esp + 12]
; 描述：内存复制函数
; ------------------------------------------------
copy_memory:
    push    ebp
    mov     ebp, esp
    push    esi
    push    ecx
    push    edi
    push    eax

    ; src
    mov     esi, [ebp + 8]
    ; size
    mov     ecx, [ebp + 12]
    ; dst
    mov     edi, [ebp + 16]

.1:
    cmp     ecx, 0
    jz      .2

    mov     al, [ds:esi]
    inc     esi

    mov     [es:edi], al
    inc     edi

    dec     ecx
    jmp     .1

.2:
    pop     eax
    pop     edi
    pop     ecx
    pop     esi
    pop     ebp
    ret


; ------------------------------------------------
; print_string
; 参数：
;   str     字符串首地址 [esp + 4]
; 描述：
;   在屏幕光标位置输出字符串
; ------------------------------------------------
print_string:
	push	ebp
	mov		ebp, esp
	push	esi
	push 	edi
	push	ebx

	mov		esi, [ebp + 8]
	mov		ah, 0fh
	mov		edi, [POINTER_POS]
	
.begin:
	lodsb
	; test 指令，用来测试 al 是否为0而不用修改 al 的值，又学到一个
	test 	al, al
	jz		.over

	; 这里还有一个换行符号判断，\n的ascii码是0ah
	cmp		al, 0ah
	jz		.newline

	mov		[gs:edi], ax
	add		edi, 2
	jmp 	.begin

.newline:
	; 换行
	push	eax

	mov		eax, edi
	mov		bl, 160
	; ax / 160	-> al:ah = 商:余数
	div		bl
	and		ax, 0ffh
	inc		ax
	mov		bl, 160
	mul		bl
	mov 	edi, eax

	pop 	eax
	jmp		.begin

.over:
	; 已经输出完毕
	mov		[POINTER_POS], edi

	pop		ebx
	pop		edi
	pop 	esi
	pop 	ebp
	ret