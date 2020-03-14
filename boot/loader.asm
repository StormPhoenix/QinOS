; ------------------------------------------------
; loader.asm 操作系统引导程序
; 本程序功能：
;	1. 加载内核代码进入内存（简直就是boot.asm的翻版）
; 	2. 初始化一些数据结构（空闲内存映射位图）
;	3. 进入保护模式
; ------------------------------------------------

; 这段代码会被加载到 0x9000:0x100 这个地方，因此编译的时候会被设置成 0100h
org	0100h

;------------------------------------------------------------------------------------------
; 相关宏定义
BASE_STACK		equ 0100h
;------------------------------------------------------------------------------------------

	jmp	START

%include	"fat12.inc"
%include	"pm.inc"
%include	"boot.inc"

; ------------------------------------------------
; 定义GDT表
GDT_START:
	DESCRIPTOR	0, 0, 0
GDT_DESC_CODE:
	; 代码段，可读可执行 | 32位代码段 | 段限长粒度4K
	DESCRIPTOR  0fffffh, 0, CODE_CR | CODE_32 | DPL0 | CODE_LIMIT_4k
GDT_DESC_DATA:
	; 数据段，可读可写 ｜ 32位代码段 ｜ 段限长粒度4K
	DESCRIPTOR 	0fffffh, 0, DATA_RW | CODE_32 | DPL0 | CODE_LIMIT_4k
GDT_DESC_VIDEO:
	; 屏幕显示数据缓冲
	DESCRIPTOR  0ffffh, 0B8000h, DATA_RW | DPL3

; GDT表的长度
GDT_LEN		equ	$ - GDT_START
; 指定GDT的位置，用于加载到gdtr
GDT_PTR:
	dw	GDT_LEN - 1
	dd	BASE_LOADER * 10h + GDT_START

SELECTOR_CODE	equ	GDT_DESC_CODE - GDT_START + TI_GDT + RPL0
SELECTOR_DATA	equ GDT_DESC_DATA - GDT_START + TI_GDT + RPL0
SELECTOR_VIDEO  equ GDT_DESC_VIDEO - GDT_START + TI_GDT + RPL3

; ------------------------------------------------

START:
	mov 	ax, cs
    mov 	ds, ax
    mov 	es, ax
	mov		ds, ax
	mov		ss, ax
	mov		sp, BASE_STACK

	call	clear_screen

	mov		dx, 0000h
	mov 	bh, 0h
	call	set_cursor

	; 检查内存信息
	mov		ebx, 0
	mov		di, MEM_INFO_BUFFER

.MEM_CHECK:
	mov		ax, 0e820h
	mov 	ecx, 20
	mov		edx, 0534d4150h
	int 	15h
	jc		.MEM_CHECK_FAILED

	add		di, 20
	inc		dword [_MEM_ARDS_NUMBER]

	cmp 	ebx, 0
	jne		.MEM_CHECK
	jmp 	.MEM_CHECK_OK

.MEM_CHECK_FAILED:
	mov		dword [_MEM_ARDS_NUMBER], 0

.MEM_CHECK_OK:

	; 打印 "Loading OS "
	mov		ax, ds
	mov		bx, MSG_LOADING_OS
	mov 	cx, 10
	mov		dx, 0000h
	call 	print_message

	; 复位软驱动
	call	reset_soft_driver

	; 将下一个需要读取的扇区保存在 [SECTOR_NO] 位置
	mov		word [SECTOR_NO], SECTOR_ROOT_DIR_BEGIN

.SEARCH_KERNEL_IN_SECTOR:
	; 查看是否已经读完了根目录占用的所有扇区
	cmp		word [REST_SECTORS], 0
	jz		.KERNEL_NOT_FOUND

	dec		word [REST_SECTORS]
	; 要先读取FAT12的根目录区，然后才能搜索
	mov		ax, BASE_KERNEL
	mov		es, ax
	mov		ax, OFFSET_KERNEL
	mov		bx, ax
	mov		ax,	[SECTOR_NO]
	mov		cl,	1
	call 	read_sector

	; 从读取的根目录区中查找是否有 kernel.bin 模块，方法是对比是否有 "KERNEL  BIN" 文件名的 字段
	; ds:si 文件名
	; es:di 根目录区数据存放位置
	mov		si,	KERNEL_NAME
	mov 	di, OFFSET_KERNEL
	cld
	; 一个扇区512B，根目录每个条目有32B，总共有16（10h）个
	mov		dx,	10h

.COMPARE_NEXT_ROOT_ENTRY:
	cmp		dx, 0
	; 如果 dx 等于 0，说明这个扇区已经检索完毕但没有找到 "KERNEL  BIN"，则继续读取下一个扇区
	jz		.READ_NEXT_SECTOR_OF_ROOT_DIR
	
	dec		dx
	; FAT12 文件系统中，根目录中，每个条目的文件名选项最大长度是11字节
	mov		cx,	11

.COMPARE_KERNEL_NAME:
	cmp		cx,	0
	; 如果11个字节和要找的 kernel 一致，则说明已经找到了
	jz		.KERNEL_FOUND
	
	dec		cx
	; [ds:si] -> al
	lodsb

	cmp		al,	byte [es:di]
	jz		.KEEP_GOING_ON

	jmp		.ON_KERNEL_NAME_DIFFERENT

.READ_NEXT_SECTOR_OF_ROOT_DIR:
	add		word [SECTOR_NO], 1
	jmp		.SEARCH_KERNEL_IN_SECTOR

.KERNEL_FOUND:
	; es:di 目前指向 kernel.bin 文件在根目录的某一项中，
	; 现在要把 es:di 重新定位到 DIR_FstClus 这个地方
	and		di, 0ffe0h
	add		di, 01ah

	; 此时 es:di 指向了 kernel.bin 的 DIR_FstClus 条目了
	; 现在要把它取出来，并且根据这个值来计算对应的扇区号
	mov		cx, word [es:di]
	; 保存序号，后面查找FAT表的时候需要用到
	push	cx
	mov 	ax, SECTORS_OF_ROOT_DIR
	add		cx, ax
	add		cx, CONSTANT_SECTOR_17

	mov		ax, BASE_KERNEL
	mov		es, ax
	mov		bx, OFFSET_KERNEL
	mov		ax, cx
	
.KEEP_LOADING:
	mov		cl, 1
	call	read_sector

	; 每读一个扇区，就显示一个 "."
	push	ax

	mov		al, '.'
	call	print_char	

	pop		ax

	; 这里要判断一下是否还需要继续读
	pop 	ax
	call	get_next_sector_no
	cmp		ax, 0fffh
	jz		.KERNEL_LOADED

	; kernel.bin 还没加载完，则继续加载
	push	ax
	mov		dx, SECTORS_OF_ROOT_DIR
	add		ax, dx
	add 	ax, CONSTANT_SECTOR_17
	add		bx, [BPB_BytsPerSec]
	jmp		.KEEP_LOADING

.KERNEL_LOADED:
	; 关闭软驱
	call	close_soft_driver
	; 这个地方显示一个 "Coming soon ... "，然后死循环
	mov		ax, ds
	mov		bx, MSG_COMING_SOON
	mov 	cx, 16
	mov		dx, 0100h
	call 	print_message

	; 预备跳入保护模式
	lgdt	[GDT_PTR]
	; 关中断
	cli
	; 开启A20地址线	
	in	al, 92h
	or	al, 00000010b
	out	92h, al
	; 修改cr0，正式开启保护模式
	mov		eax, cr0
	or		eax, 1
	mov		cr0, eax
	jmp		dword SELECTOR_CODE:(BASE_LOADER*10h + PM_MODE_START)

.KEEP_GOING_ON:
	inc		di
	jmp		.COMPARE_KERNEL_NAME

.ON_KERNEL_NAME_DIFFERENT:
	; 比较 "KERNEL  BIN" 名字过程中，如果发现不一致，则先重置 di 和 si，然后在比较下一个32B的根目录项目
	and		di,	0ffe0h
	add		di,	20h
	mov		si,	KERNEL_NAME
	jmp		.COMPARE_NEXT_ROOT_ENTRY

.KERNEL_NOT_FOUND:
	; 这个地方显示一个 "Kernel NOT FOUND"，然后死循环
	mov		ax, ds
	mov		bx, MSG_KERNEL_NOT_FOUND
	mov 	cx, 16
	mov		dx, 0000h
	call 	print_message
	jmp $


;------------------------------------------------------------------------------------------
; 函数：print_message
; 参数：ax 字符串段地址
; 	   bx 字符串偏移地址
;	   cx 字符串长度
;	   dh 显示位置-行
;	   dl 显示位置-列
; 描述：在屏幕上显示字符信息
;------------------------------------------------------------------------------------------
print_message:
	push	es
	push	bp

	mov		es, ax
	mov		bp, bx

    mov		ax, 01301h		; AH = 13,  AL = 01h
	mov		bx, 000ch		; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)
	; mov		dx, 1000h
	int		10h				; int 10h
	
	pop		bp
	pop		es
	
	ret


;------------------------------------------------------------------------------------------
; 函数：reset_soft_driver
; 描述：复位软驱
;------------------------------------------------------------------------------------------
reset_soft_driver:
	push	ax
	push	dx
	
	xor		ah, ah
	xor		dl, dl
	int 13h

	pop 	dx
	pop		ax

	ret


;------------------------------------------------------------------------------------------
; 函数：close_soft_driver
; 描述：关闭软驱
;------------------------------------------------------------------------------------------
close_soft_driver:
	push	dx
	push 	ax

	mov	dx, 03F2h
	mov	al, 0
	out	dx, al

	pop 	ax
	pop	dx
	ret


;------------------------------------------------------------------------------------------
; 函数：read_sector
; 		
; 参数： ax 给定的扇区号
;		cl 读取的扇区数量
; 		es:bx 指定的缓冲区，用于存放读取的磁盘块
; 
; 描述：给定扇区号，读取软盘中的对应扇区
; 		int 13h 读取软盘扇区到 es:bx 指定的缓冲区中
; 		ah = 02h
; 		al = 读取的扇区数
; 		ch = 磁道号
; 		cl = 起始扇区号
; 		dh = 磁头号
; 		dl = 驱动器号
; 		es:bx = 数据缓冲区
;------------------------------------------------------------------------------------------
read_sector:
	push    dx
	push 	bp
	mov		bp, sp

	sub		esp, 2					; 调用中断时，需要用到 cl 寄存器，这里将 cl 里面的参数保存起来
	mov		byte [bp - 2], cl

	push	bx						; 保存 bx
	mov		bl, [BPB_SecPerTrk]		; 读取磁道数
	div		bl						; 扇区号除以磁道数，bl 是8位，被除数在 ax 里面存放，共16位
									; 如果除数是8位对，则 al 存放商， ah 存储余数
	inc		ah
	mov		cl, ah					; cl 存放起始扇区号
	mov 	dh, al
	and		dh, 1					; dl 存放磁头号
	shr 	al, 1
	mov		ch, al					; ch 存放磁道号
	mov 	dl, [BS_DrvNum]			; dl 存放驱动器号

	pop		bx						; 恢复bx
.KEEP_READING:
	mov		ah, 2
	mov		al, [bp - 2]
	int 	13h						; 发起中断读取软盘数据
	jc 		.KEEP_READING			; 如果读取失败 EFLAGS 的 cf 位将置1
	
	add		esp, 2
	pop 	bp
	pop 	dx

	ret


;------------------------------------------------------------------------------------------
; 函数：get_next_sector_no
; 描述：给定序号，从FAT12表中查找对应的下一个扇区。
;	   FAT12 里面，每一个序号对应一个扇区，起始序号是从2开始
; 参数：ax 给定的序号
; 返回值：ax 存放下一个序号
;------------------------------------------------------------------------------------------
get_next_sector_no:
	push 	es
	push	bx
	push	dx

	push	ax
	; 设置 es 的值，用于读取扇区
	mov		ax, BASE_KERNEL
	; 这里规定FAT表的扇区放在 0x8f00:0x0000 这个地方
	sub		ax, 0100h
	mov		es, ax
	pop		ax

	mov		byte [IF_ODD], 0
	; 先计算序号对应的 FAT Entry 在 FAT表里面的偏移值
	; 计算方法是 ax * 1.5 byte
	mov		bx, 3
	; ax * 3 -> dx:ax
	mul 	bx	
	mov		bx, 2
	; ax / 2 -> ax 商 dx 余数
	div		bx
	; 如果余数不为0，说明偏移值刚好位于半个byte之间
	cmp		dx, 0
	jz		.READ_FAT
	; 处理奇数的情况，如果是奇数，就在此处存1，方便后续判断
	mov		byte [IF_ODD], 1

.READ_FAT:
	; 偏移值是字节的倍数
	; ax 里面保存的是偏移值，现在看这个偏移值跨了几个扇区
	xor		dx, dx
	mov		bx, [BPB_BytsPerSec]
	; dx:ax / BPB_BytsPerSec
	; ax 保存扇区号
	; dx 保存扇区内的偏移
	div		bx
	
	; 准备读取FAT的扇区
	; 开始设置调用参数
	mov		bx, 0
	mov		cl, 2
	add		ax, SECTOR_FAT1
	call	read_sector

	; FAT表中的项已经读到 es:bx 上了，现在取出这个项
	add		bx, dx
	mov		ax, [es:bx]
	; 判断FAT项，是从整数byte开始的还是从半个byte开始算的
	cmp		byte [IF_ODD], 1
	jnz		.NOT_ODD
	shr		ax, 4

.NOT_ODD:
	and 	ax, 0fffh

	pop		dx
	pop		bx
	pop 	es

	ret


;------------------------------------------------------------------------------------------
; 函数：clear_screen
; 描述：清空屏幕
;------------------------------------------------------------------------------------------
clear_screen:
	push	ax
	push	bx
	push	cx
	push 	dx

	mov		ax, 0600h
	; 左上角的行号
	mov 	ch, 0   
	; 左上角的列号
    mov 	cl, 0   
	; 右下角的行号
    mov 	dh, 24  
	;右下角的行号
    mov 	dl, 79  
	; 属性为黑底白字
    mov 	bh, 0x07
	int 	10h

	pop		dx
	pop		cx
	pop 	bx
	pop		ax
	ret


;------------------------------------------------------------------------------------------
; 函数：print_char
; 参数： al 字符
; 		bh 显示页码
; 		bl 属性(文本模式)或颜色(图形模式)
; 描述：在光标处显示字符
;------------------------------------------------------------------------------------------
print_char:
	push	ax
	push	bx
	
	mov		ah, 0eh
	mov		bl, 0fh
	int		10h

	pop		bx
	pop		ax

	ret


;------------------------------------------------------------------------------------------
; 函数：set_cursor
; 参数： 
; 	bh 显示页码
;	dh 行(Y坐标)
;   dl 列(X坐标)
; 描述：设置光标位置
;------------------------------------------------------------------------------------------
set_cursor:
	push 	ax

	mov		ah, 02h
	int		10h

	pop		ax
	ret


;------------------------------------------------------------------------------------------
; 数据区
;------------------------------------------------------------------------------------------

; 临时存放扇区号
SECTOR_NO:
 	dw	0
REST_SECTORS:
	dw  SECTORS_OF_ROOT_DIR

; OS 模块名
KERNEL_NAME:
	db	"KERNEL  BIN", 0

IF_ODD:
	db	0

MSG_LOADING_OS:
	db "Loading OS"

MSG_KERNEL_NOT_FOUND:
	db "Kernel NOT FOUND"

MSG_COMING_SOON:
	db "Coming soon ... "

; -----------------------------------------------
; 进入保护模式，后续代码是32位的，需要4 byte对齐
; -----------------------------------------------
[SECTION .pm_text]
; 4字节对齐
ALIGN	32
; 之后的指令用32位编制
[BITS	32]

PM_MODE_START:
	mov 	ax, SELECTOR_DATA
	mov		ds, ax
	mov		es, ax
	mov 	fs, ax
	mov 	ss, ax
	mov		esp, STACK_BOTTOM

	mov		ax, SELECTOR_VIDEO
	mov 	gs, ax

	call	print_memory_info

	call	print_newline
	push	STRING_START_PAGING
	call	print_string
	add 	esp, 4
	
	call	setup_paging

	call	print_newline
	push	STRING_SETUP_KERNEL
	call	print_string
	add		esp, 4

	call 	setup_kernel

	jmp		SELECTOR_CODE:KERNEL_ENTRY_POINT


;------------------------------------------------------------------------------------------
; setup_kernel
; 描述：内核是elf格式，根据elf信息重新放置内核位置
;
; e_phnum program header table 数目，offset = 2ch，size = 2 byte
; e_phoff program header table 在文件中的偏移值，offset = 1ch，size = 4 byte
; e_phentsize program header 大小，offset = 2ah，size = 2 byte
;
; p_type 类型，offset = 0h，size = 4 byte
; p_offset 段的第一个字节在文件中的偏移，offset = 4h，size = 4 byte
; p_vaddr 段在内存段虚拟地址，offset = 8h，size = 4 byte
; p_filesz 段在文件中段长度，offset = 10h，size = 4 byte
;------------------------------------------------------------------------------------------
setup_kernel:
	push	ecx
	push	esi
	push 	eax
	push	edx
	push    edi

	; 得到 program header 数目
	; e_phnum 只有两个字节，需要扩展到32位
	mov		cx, word [BASE_KERNEL * 10h + OFFSET_KERNEL + 2ch]
	movzx	ecx, cx
	; e_phoff
	mov		esi, dword [BASE_KERNEL * 10h + OFFSET_KERNEL + 1ch]
	add		esi, BASE_KERNEL * 10h + OFFSET_KERNEL
	; e_phentsize，不过一般来说这个值都是 020h
	mov		bx, word [BASE_KERNEL * 10h + OFFSET_KERNEL + 2ah]
	movzx	ebx, bx

	; -------- 检查每一个 program header --------
.1:
	; 得到 p_type
	; 这里 p_type 取值我没查ELF相关资料，这里和书上处理一样吧
	mov		eax, [esi + 0h]
	cmp		eax, 0
	jz		.2

	; p_vaddr
	push	dword [esi + 08h]
	; p_filesz
	push	dword [esi + 10h]
	; p_offset
	mov		eax, [esi + 04h]
	add		eax, BASE_KERNEL * 10h + OFFSET_KERNEL
	push	eax
	call	copy_memory
	add		esp, 12

.2:
	add		esi, ebx
	dec		ecx
	cmp		ecx, 0
	jnz		.1

    ; -------- 初始化 .bss 段 --------
    ; 需要查看 section_header_table
    ; s_addr .bss 段在内存中的起始地址
    ; s_size .bss 的大小

    ; 定位到 section_header_table
    ; 取 e_shoff，得到 section_header_table 偏移
    mov     esi, dword [BASE_KERNEL * 10h + OFFSET_KERNEL + 20h]
    add     esi, BASE_KERNEL * 10h + OFFSET_KERNEL
    ; 取 e_shnum，得到 section_header_table entry 数量
    mov     cx, word [BASE_KERNEL * 10h + OFFSET_KERNEL + 30h]
    movzx   ecx, cx

    ; 开始检查 section_header_table 中的每一个 entry
.check_section:
    ; 取出 s_type
    mov     eax, [esi + 4h]
    cmp     eax, 8
    jnz     .loop_check_section

    ; 取出 s_addr，得到内存中的偏移
    mov     edi, dword [esi + 0xc]
    ; 取出 s_size，得到 bss 段大小
    mov     edx, dword [esi + 0x14]
    mov     al, 0
.init_bss:
    ; 初始化 .bss 段
    mov     byte [es:edi], al

    inc     edi
    dec     edx
    cmp     edx, 0
    jnz     .init_bss

.loop_check_section:
    add     esi, 0x28
    dec     ecx
    cmp     ecx, 0
    jnz     .check_section

    pop     edi
	pop		edx
	pop		eax
	pop		esi
	pop 	ecx
	ret


;------------------------------------------------------------------------------------------
; copy_memory
; 参数：
;	src	位置[esp + 4]，大小 4 byte
;   num 复制字节数目 位置[esp + 8]，大小 4 byte
;   dst 位置[esp + 12]，大小 4 byte
; 描述：复制内存，从src处复制num个字节到dst
;------------------------------------------------------------------------------------------
copy_memory:
	push	ebp
	mov		ebp, esp
	push 	esi
	push 	ecx
	push 	edi
	push 	eax

	; 取出 src
	mov		esi, [ebp + 8]
	mov 	ecx, [ebp + 12]
	mov		edi, [ebp + 16]

.1:
	cmp		ecx, 0
	jz		.2

	; TODO 下面这两行代码能不能替换成 lodsb 和 stosb 以及loop
	mov		al, [ds:esi]
	inc		esi

	mov 	byte [es:edi], al 
	inc 	edi

	dec 	ecx
	jmp		.1	

.2:
	pop 	eax
	pop 	edi
	pop 	ecx
	pop 	esi
	pop		ebp
	ret


;------------------------------------------------------------------------------------------
; print_memory_info
; 描述：打印内存信息，并且计算内存容量
;------------------------------------------------------------------------------------------
print_memory_info:
	push	ecx
	push 	esi
	push 	edx
	push	eax
	push	edi

	mov		esi, OFFSET_MEM_INFO_BUFFER
	mov		ecx, [MEM_ARDS_NUMBER]
.READ_ARDS:
	mov 	edx, 5
	mov 	edi, ARDS_STRUCT

.READ_ARDS_STRUCT:
	push	dword [esi]
	call	print_int
	pop		dword eax
	stosd
	add		esi, 4
	dec 	edx

	cmp		edx, 0
	jnz		.READ_ARDS_STRUCT

	call	print_newline
	cmp 	dword [ARDS_TYPE], 1
	jne		.KEEP_READING_ARDS

	mov		eax, [BASE_ADDR_LOW]
	add		eax, [LENGTH_LOW]
	cmp		eax, [MEM_SIZE]

	jb		.KEEP_READING_ARDS
	mov		[MEM_SIZE], eax

.KEEP_READING_ARDS:
	loop	.READ_ARDS

	call	print_newline
	push	STRING_MEMORY_SIZE
	call 	print_string
	add		esp, 4

	push	dword [MEM_SIZE]
	call	print_int
	add		esp, 4

	call 	print_newline

	pop 	edi
	pop 	eax
	pop 	edx
	pop 	esi
	pop		ecx

	ret


;------------------------------------------------------------------------------------------
; setup_paging
; 描述：设置页目录和页表
; 注：PDE = 页目录项，PTE = 页表项目
;------------------------------------------------------------------------------------------
setup_paging:
	push	eax
	push 	ebx
	push	edx
	push	ecx
	push	es
	push 	edi

	; 计算有多少个PDE（对应有多少个页表）
	; 不加上 xor 这条指令，会有溢出的错误
	; div ebx相当于edx:eax / ebx 得到的值应该是32位范围内的
	; 如果edx不清0，最后的商用eax可能装不下，发生溢出错误
	xor 	edx, edx
	mov		eax, [MEM_SIZE]
	mov		ebx, 0400000h
	div		ebx
	test	edx, edx
	jz		.1
	inc		eax

.1:
	mov		ecx, eax

	; 保存下PDE数量
	push	ecx
	; 设置页目录
	mov		eax, SELECTOR_DATA
	mov		es, eax
	mov		edi, BASE_PAGE_DIR

	; xor		eax, eax
	; TODO 这里 PDE_U 我认为应该替换成 PDE_S
	mov		eax, BASE_PAGE_TABLE | PDE_P | PDE_RW | PDE_U

.2:
	stosd
	add		eax, 01000h
	loop	.2

	; 计算所有页表加起来的项数
	pop		eax
	mov		ebx, 1024
	mul		ebx
	mov		ecx, eax

	; 设置页表项
	mov		edi, BASE_PAGE_TABLE
	xor 	eax, eax
	mov 	eax, PTE_P | PTE_RW | PTE_U

.3:
	stosd
	add		eax, 01000h
	loop	.3

	; 开启分页
	mov		eax, BASE_PAGE_DIR
	mov		cr3, eax
	mov		eax, cr0
	or		eax, 80000000h
	mov		cr0, eax
	; -----------------------------------------
	; 在i386中，这两条指令只开启分页，但没有刷新指令流水
	; 所以下一条指令 jmp 实际上取但是实际物理地址而不是
	; 分页得到的地址，所以下一条指令需要用jmp和nop来刷
	; 新流水线。
	; -----------------------------
	jmp		short .4

.4:
	nop

	pop 	edi
	pop		es
	pop		ecx
	pop 	edx
	pop		ebx
	pop 	eax
	ret


;------------------------------------------------------------------------------------------
; print_hex
; 参数：al保存数字
; 描述：将al保存的数字按照16进制输出
;------------------------------------------------------------------------------------------
print_hex:
	push	edx
	push	edi
	push	ecx

	; 黑底白字
	mov		ah, 0fh
	mov		dl, al
	mov		edi, [SCREEN_POS]

	; 先输出高4位
	shr		al, 4
	mov		ecx, 2

.1:
	and		al, 01111b
	cmp		al, 9
	ja		.2
	add		al, '0'
	jmp		.3

.2:
	; 数字比9大，说明要输出字母
	sub		al, 0Ah
	add		al, 'A'

.3:
	; 将ax放入显示缓冲区域，ah是显示属性，al是显示内容
	mov		[gs:edi], ax
	add 	edi, 2
	mov		al, dl
	loop	.1

	mov		[SCREEN_POS], edi

	pop 	ecx
	pop 	edi
	pop		edx
	ret


;------------------------------------------------------------------------------------------
; print_int
; 参数：一个数字，存放在dword [esp + 4]
; 描述：打印内存信息
; 注意：该函数用于段内跳转
;------------------------------------------------------------------------------------------
print_int:
	push	ebp
	mov		ebp, esp
	push 	eax

	; 打印 24 - 31 位
	mov		eax, [ebp + 8]
	shr		eax, 24
	call	print_hex

	; 打印 16 - 23 位置
	mov		eax, [ebp + 8]
	shr		eax, 16
	call	print_hex
	
	; 打印 8 - 15 位置
	mov		eax, [ebp + 8]
	shr		eax, 8
	call	print_hex

	; 打印 0 - 7 位置
	mov		eax, [ebp + 8]
	call	print_hex

	; 0000b: 黑底    0111b: 灰字
	mov		ah, 07h			
	mov		al, 'h'
	push	edi
	mov		edi, [SCREEN_POS]
	mov		[gs:edi], ax
	; 加4用来在屏幕上右移两个字符
	add		edi, 4
	mov		[SCREEN_POS], edi
	pop		edi

	pop		eax
	pop		ebp
	ret


;------------------------------------------------------------------------------------------
; print_string
; 参数：字符串首地址，存放在 [esp + 4]
; 描述：字符串
;------------------------------------------------------------------------------------------
print_string:
	push	ebp
	mov		ebp, esp
	push	esi
	push 	edi
	push	ebx

	mov		esi, [ebp + 8]
	mov		ah, 0fh
	mov		edi, [SCREEN_POS]
	
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
	mov		[SCREEN_POS], edi

	pop		ebx
	pop		edi
	pop 	esi
	pop 	ebp
	ret


;------------------------------------------------------------------------------------------
; print_newline
; 描述：打印'\n'
;------------------------------------------------------------------------------------------

print_newline:
	push	STRING_NEWLINE
	call	print_string
	add		esp, 4
	ret


[SECTION .pm_data]
; 堆栈空间 1K 大小
	times	512	db	0
STACK_BOTTOM	equ	BASE_LOADER * 10h + $

; ARDS 的数量
_MEM_ARDS_NUMBER:
	dd	0
MEM_ARDS_NUMBER			equ BASE_LOADER * 10h + _MEM_ARDS_NUMBER

; 存放内存地址范围描述符结构 ARDS
MEM_INFO_BUFFER:
	times	256	db	0

OFFSET_MEM_INFO_BUFFER	equ	BASE_LOADER * 10h + MEM_INFO_BUFFER

; 每次处理一个ARDS的时候，就把ARDS放到这个地方
_ARDS_STRUCT:
	_BASE_ADDR_LOW:		dd	0
	_BASE_ADDR_HIGH:	dd 	0
	_LENGTH_LOW:		dd 	0
	_LENGTH_HIGH:		dd  0
	_ARDS_TYPE:			dd	0

ARDS_STRUCT			equ	BASE_LOADER * 10h + _ARDS_STRUCT
BASE_ADDR_LOW		equ	BASE_LOADER * 10h + _BASE_ADDR_LOW
BASE_ADDR_HIGH		equ	BASE_LOADER * 10h + _BASE_ADDR_HIGH
LENGTH_LOW			equ	BASE_LOADER * 10h + _LENGTH_LOW
LENGTH_HIGH			equ	BASE_LOADER * 10h + _LENGTH_HIGH
ARDS_TYPE			equ	BASE_LOADER * 10h + _ARDS_TYPE

_MEM_SIZE:
	dd	0
MEM_SIZE			equ	BASE_LOADER * 10h + _MEM_SIZE

_STRING_MEMORY_SIZE:
	db "Memory size: ", 0
STRING_MEMORY_SIZE	equ BASE_LOADER * 10h + _STRING_MEMORY_SIZE

_STRING_NEWLINE:
	db 0ah, 0
STRING_NEWLINE 		equ BASE_LOADER * 10h + _STRING_NEWLINE

_STRING_START_PAGING:
	db "Setup paging ... ", 0
STRING_START_PAGING equ BASE_LOADER * 10h + _STRING_START_PAGING

_STRING_SETUP_KERNEL:
	db "Setup kernel ... ", 0
STRING_SETUP_KERNEL equ BASE_LOADER * 10h + _STRING_SETUP_KERNEL

_SCREEN_POS:
	dd	(80 * 4 + 0) * 2
SCREEN_POS			equ BASE_LOADER * 10h + _SCREEN_POS