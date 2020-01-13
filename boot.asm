%include    "section.inc"

LIMIT_GDT_CODE_LIMIT   equ end_text - begin_text
LIMIT_GDT_DATA_LIMIT   equ end_text - begin_text

LIMIT_LDT0      equ ldt0_end - ldt0

org     07c00h

[bits   16]
begin_text:
    jmp     start

; gdt 表
gdt:
    DESCRIPTOR  0, 0, 0
gdt_desc_code:
    DESCRIPTOR  LIMIT_GDT_CODE_LIMIT, 0, CODE_C + CODE_32
gdt_desc_data:
    DESCRIPTOR  LIMIT_GDT_DATA_LIMIT, 0, DATA_R
gdt_desc_video:
    DESCRIPTOR  0ffffh, 0b8000h, DATA_RW
gdt_desc_ldt0:
    DESCRIPTOR  LIMIT_LDT0, 0, SYS_LDT
gdt_len     equ $ - gdt
gdt_ptr:
    dw  gdt_len - 1
    dd  gdt

; gdt 选择符
SELECTOR_GDT_CODE   equ gdt_desc_code - gdt
; 显存选择符
SELECTOR_GDT_VIDEO  equ gdt_desc_video - gdt
; 数据段选择符
SELECTOR_GDT_DATA   equ gdt_desc_data - gdt
; ldt0 选择符号，供 lldt 指令使用
SELECTOR_GDT_LDT0   equ gdt_desc_ldt0 - gdt

start:
    ; 初始化代码
    ; 重新设置 GDT 表
    ; 原始版本将 GDT 表里面的代码段描述符的基地址修改成0，导致报错，后来进过排查是 代码段限长没有修改更大，导致访问越界
    ; 设置 GDT 表中的代码段描述符
    xor eax, eax
    mov ax, cs
    add eax, label_protect_mode
    mov word [gdt_desc_code + 2], ax
    shr eax, 16
    mov byte [gdt_desc_code + 4], al
    mov byte [gdt_desc_code + 7], ah

    ; 设置 LDT 表中数据段描述符
    xor eax, eax
    mov ax, ds
    add eax, label_protect_mode
    mov word [gdt_desc_data + 2], ax
    shr eax, 16
    mov byte [gdt_desc_data + 4], al
    mov byte [gdt_desc_data + 7], ah

    ; 设置 GDT 表中的 LDT 描述符
    xor eax, eax
    mov eax, ldt0
    mov word [gdt_desc_ldt0 + 2], ax
    shr eax, 16
    mov byte [gdt_desc_ldt0 + 4], al
    mov byte [gdt_desc_ldt0 + 7], ah

    ; 设置 LDT 表中的代码段描述符
    xor eax, eax
    mov eax, task0_code
    mov word [ldt0_desc_code + 2], ax
    shr eax, 16
    mov byte [ldt0_desc_code + 4], al
    mov byte [ldt0_desc_code + 7], ah

    ; 设置 LDT 表中的数据段描述符
    xor eax, eax
    mov eax, task0_data
    mov word [ldt0_desc_data + 2], ax
    shr eax, 16
    mov byte [ldt0_desc_data + 4], al
    mov byte [ldt0_desc_data + 7], ah

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

    ; 跳转到 SELECTOR_GDT_CODE 选择符指定的地方
    jmp dword   SELECTOR_GDT_CODE:0

[bits   32]

label_protect_mode:
    mov ax, SELECTOR_GDT_VIDEO
    mov gs, ax

    mov ax, SELECTOR_GDT_DATA
    mov es, ax

    mov eax, print_message - label_protect_mode
    mov esi, eax

    mov edx, 20
	mov	ah, 0Ch			; 0000: 黑底    1100: 红字

    mov cx, 21
repeat:
    mov byte al, [es:esi]
    ; 屏幕第 20 行, 第 dx 列
    mov edi, edx
    add edi, 80 * 20
    add edi, edi

    mov [gs:edi], ax

    inc esi
    inc edx
    dec cx

    cmp cx, 0
    jnz repeat

    ; lldt 指令必须要在保护模式里面执行，在实模式下面执行会报错
    ; 错误写法 lldt    SELECTOR_GDT_LDT0
    mov ax, SELECTOR_GDT_LDT0
    lldt    ax

    ; jmp $
    ; 这里只能采用直接写入显存的方式，因为还没有加上中断描述表，所以不能在保护模式下使用中断
    ; mov	ax, print_message
	; mov	bp, ax			; ES:BP = 串地址
	; mov	cx, 21			; CX = 串长度
	; mov	ax, 01301h		; AH = 13,  AL = 01h
	; mov	bx, 000ch		; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)
	; mov	dl, 0
	; int	10h			; 10h 号中断
    ; jmp $
    jmp SELECTOR_LDT0_CODE:0

print_message:
    db  "Hello, Storm Phoenix."
end_text:

LIMIT_LDT0_CODE     equ task0_code_end - task0_code
LIMIT_LDT0_DATA     equ taks0_data_end - task0_data

; LDT 表
ldt0:
ldt0_desc_code:
    DESCRIPTOR  LIMIT_LDT0_CODE, 0, CODE_CR + CODE_32
ldt0_desc_data:
    DESCRIPTOR  LIMIT_LDT0_DATA, 0, DATA_R
ldt0_end:

; LDT0 的数据段选择符
SELECTOR_LDT0_DATA  equ ldt0_desc_data - ldt0 + TI_LDT
; LDT0 的代码段选择符
SELECTOR_LDT0_CODE  equ ldt0_desc_code - ldt0 + TI_LDT

task0_code:
    ; 从 [es:esi] 指定的位置复制数据到 [gs:edi] 指向到显存中
    mov ax, SELECTOR_GDT_VIDEO
    mov gs, ax

    mov ax, SELECTOR_LDT0_DATA
    mov es, ax

    mov eax, 0
    mov esi, eax

    mov edx, 20
    mov ah, 0ch

    mov cx, 19

task0_repeat:
    mov byte al, [es:esi]

    mov edi, edx
    add edi, 80 * 22
    add edi, edi

    mov [gs:edi], ax

    inc esi
    inc edx
    dec cx
    cmp cx, 0
    jnz task0_repeat
    jmp $
task0_code_end:

task0_data:
    db  "Enter in LDT0 Code."
taks0_data_end:

times   510 - ($ - $$)   db  0
dw  0xaa55