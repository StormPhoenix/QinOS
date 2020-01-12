%include    "section.inc"

section_limit  equ end_text - begin_text

org     07c00h
begin_text:
    jmp     start

; gdt 表
gdt_start:
    GDT_DESCRIPTOR  0, 0, 0
gdt_desc_code:
    GDT_DESCRIPTOR  section_limit, 0, CODE_C + CODE_32
gdt_desc_data:
    GDT_DESCRIPTOR  section_limit, 0, DATA_R
gdt_desc_video:
    GDT_DESCRIPTOR  0ffffh, 0b8000h, DATA_RW
gdt_len     equ $ - gdt_start
gdt_ptr:
    dw  gdt_len - 1
    dd  gdt_start

; gdt 选择符
selector_code   equ gdt_desc_code - gdt_start
; 显存选择符
selector_video  equ gdt_desc_video - gdt_start
; 数据段选择符
selector_data   equ gdt_desc_data - gdt_start

start:
    ; 重新设置 gdt 表中的代码段、和数据段
    ; 原始版本将 gdt 表里面的代码段描述符的基地址修改成0，导致报错，后来进过排查是 代码段限长没有修改更大，导致访问越界
    xor eax, eax
    mov ax, cs 
    add eax, label_protect_mode
    mov word [gdt_desc_code + 2], ax
    shr eax, 16
    mov byte [gdt_desc_code + 4], al
    mov byte [gdt_desc_code + 7], ah

    xor eax, eax
    mov ax, ds
    add eax, label_protect_mode
    mov word [gdt_desc_data + 2], ax
    shr eax, 16
    mov byte [gdt_desc_data + 4], al
    mov byte [gdt_desc_data + 7], ah

    ; 以下预备切换到保护模式

    ; 加载 gdtr
    lgdt    [gdt_ptr]

    ; 关闭中断
    cli

    ; 程序预备访问 32 位地址，开始 A20 地址线
    in  al, 92h
    or  al, 00000010b
    out	92h, al

    ; 开始保护模式
    mov eax, cr0
    ; add eax, 1
    or  eax, 1
    mov cr0, eax

    ; 跳转到 selector_code 选择符指定的地方
    jmp dword   selector_code:0

[BITS   32]

label_protect_mode:
    mov ax, selector_video
    mov gs, ax

    mov ax, selector_data
    mov es, ax

    mov eax, print_message - label_protect_mode
    mov esi, eax

    mov edx, 20
	mov	ah, 0Ch			; 0000: 黑底    1100: 红字

    mov cx, 21

repeat:
    mov byte al, [es:esi]
    ; 屏幕第 11 行, 第 dx 列
    mov edi, edx
    add edi, 80 * 20
    add edi, edi

    mov [gs:edi], ax

    inc esi
    inc edx
    dec cx

    cmp cx, 0
    jnz repeat

    jmp $
    ; 这里只能采用直接写入显存的方式，因为还没有加上中断描述表，所以不能在保护模式下使用中断
    ; mov	ax, print_message
	; mov	bp, ax			; ES:BP = 串地址
	; mov	cx, 21			; CX = 串长度
	; mov	ax, 01301h		; AH = 13,  AL = 01h
	; mov	bx, 000ch		; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)
	; mov	dl, 0
	; int	10h			; 10h 号中断
    ; jmp $

print_message:
    db  "Hello, Storm Phoenix."

end_text:
times   510 - ($ - $$)   db  0
dw  0xaa55