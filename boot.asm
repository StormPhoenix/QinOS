
    org 0x7c00

;------------------------------------------------------------------------------------------
; 相关宏定义

; 堆栈对基地址
BASE_STACK			equ	07c00h

; 基地址，存放读取扇区，最终将用来存放 loader.bin
BASE_LOADER			equ	09000h
; 偏移地址，存放读取扇区在段内的偏移值，最终用来存放 loader.bin
OFFSET_LOADER		equ	0100h

; 根目录位于的扇区
SECTOR_ROOT_DIR_BEGIN		equ	19

; 根目录占用的扇区数
; 数目14是根据BPB数据机构中的 BPB_RootEntCnt 计算得到的，每个文件都要在 ROOT_DIR 里面占用一个 32B 的位置
SECTORS_OF_ROOT_DIR	equ	14

; 第一个FAT表位于的扇区
SECTOR_FAT1			equ 1

; 通过序号计算扇区，假设序号是 x，扇区是 sector，则有
; sector = x + SECTOR_ROOT_DIR_BEGIN(19) + SECTORS_OF_ROOT_DIR(14) - 2
; 这里 SECTOR_ROOT_DIR_BEGIN - 2 是固定值17，为了避免多余计算，这里定义了一个17
CONSTANT_SECTOR_17	equ	17
;------------------------------------------------------------------------------------------


    jmp short start
	; nop 不可少
	; 参考：
	; https://www.cnblogs.com/lao3/p/3730476.html
	; https://blog.csdn.net/begginghard/article/details/7284834
    nop

    ; 这个地方参考了书中的操作系统，用的是FAT12文件系统，目的是方便向镜像中添加新文件，
	; 所以引导镜像的第一个扇区，既是作为引导扇区，也是作为操作系统可识别的FAT12文件系统
    ; 也方便程序运行过程中读取文件，所以下面读取 loader 模块的代码需要参考FAT12文件系统
	; 	的格式来编写
    
    ; FAT12 磁盘头
    BS_OEMName		DB 'fk virus'	; OEM String, 必须 8 个字节
	BPB_BytsPerSec	DW 512		; 每扇区字节数
	BPB_SecPerClus	DB 1		; 每簇多少扇区
	BPB_RsvdSecCnt	DW 1		; Boot 记录占用多少扇区
	BPB_NumFATs		DB 2		; 共有多少 FAT 表
	BPB_RootEntCnt	DW 224		; 根目录文件数最大值
	BPB_TotSec16	DW 2880		; 逻辑扇区总数
	BPB_Media		DB 0xF0		; 媒体描述符
	BPB_FATSz16		DW 9		; 每FAT扇区数
	BPB_SecPerTrk	DW 18		; 每磁道扇区数
	BPB_NumHeads	DW 2		; 磁头数(面数)
	BPB_HiddSec		DD 0		; 隐藏扇区数
	BPB_TotSec32	DD 0		; wTotalSectorCount为0时这个值记录扇区数
	BS_DrvNum		DB 0		; 中断 13 的驱动器号
	BS_Reserved1	DB 0		; 未使用
	BS_BootSig		DB 29h		; 扩展引导标记 (29h)
	BS_VolID		DD 0		; 卷序列号
	BS_VolLab		DB 'fuck  virus'; 卷标, 必须 11 个字节
	BS_FileSysType	DB 'FAT12   '	; 文件系统类型, 必须 8个字节  

start:
    mov 	ax, cs
    mov 	ds, ax
    mov 	es, ax
	mov		ds, ax
	mov		sp, BASE_STACK

	; 打印信息
	mov		ax, 0
	mov		bx, WELCOME_MSG
	mov 	cx, LENGTH_WELCOME
	mov		dx, 1200h
	call	print_message

	; 复位软驱
	call	restore_soft_driver

	; 将下一个需要读取的扇区保存在 [SECTOR_NO] 位置
	mov		word [SECTOR_NO], SECTOR_ROOT_DIR_BEGIN

.SEARCH_LOADER_IN_SECTOR:
	; 查看是否已经读完了根目录占用的所有扇区
	cmp		word [REST_SECTORS], 0
	jz		.LOADER_NOT_FOUND

	dec		word [REST_SECTORS]
	; 要先读取FAT12的根目录区，然后才能搜索
	mov		ax, BASE_LOADER
	mov		es, ax
	mov		ax, OFFSET_LOADER
	mov		bx, ax
	mov		ax,	[SECTOR_NO]
	mov		cl,	1
	call 	read_sector

	; 从读取的根目录区中查找是否有 loader.bin 模块，方法是对比是否有 loader.bin 文件名的 字段
	; ds:si 文件名
	; es:di 根目录区数据存放位置
	mov		si,	LOADER_NAME
	mov 	di, OFFSET_LOADER
	cld
	; 一个扇区512B，根目录每个条目有32B，总共有16（10h）个
	mov		dx,	10h

.COMPARE_NEXT_ROOT_ENTRY:
	cmp		dx, 0
	; 如果 dx 等于 0，说明这个扇区已经检索完毕但没有找到 loader，则继续读取下一个扇区
	jz		.READ_NEXT_SECTOR_OF_ROOT_DIR
	
	dec		dx
	; FAT12 文件系统中，根目录中，每个条目的文件名选项最大长度是11字节
	mov		cx,	11

.COMPARE_LOADER_NAME:
	cmp		cx,	0
	; 如果11个字节和要找的 loader 一致，则说明已经找到了
	jz		.LOADER_FOUND
	
	dec		cx
	; [ds:si] -> al
	lodsb

	cmp		al,	byte [es:di]
	jz		.KEEP_GOING_ON

	jmp		.ON_LOADER_NAME_DIFFERENT

.READ_NEXT_SECTOR_OF_ROOT_DIR:
	add		word [SECTOR_NO], 1
	jmp		.SEARCH_LOADER_IN_SECTOR

.LOADER_FOUND:
	; 这里显示一下 loader.bin 找到的信息
	mov		ax, 0
	mov		bx, MSG_LOADER_FOUND
	mov 	cx, LENGTH_LOADER_FOUND
	mov		dx, 1300h
	call 	print_message

	; es:di 目前指向 loader.bin 文件在根目录的某一项中，
	; 现在要把 es:di 重新定位到 DIR_FstClus 这个地方
	and		di, 0ffe0h
	add		di, 01ah

	; 此时 es:di 指向了 loader.bin 的 DIR_FstClus 条目了
	; 现在要把它取出来，并且根据这个值来计算对应的扇区号
	mov		cx, word [es:di]
	; 保存序号，后面查找FAT表的时候需要用到
	push	cx
	mov 	ax, SECTORS_OF_ROOT_DIR
	add		cx, ax
	add		cx, CONSTANT_SECTOR_17

	mov		ax, BASE_LOADER
	mov		es, ax
	mov		bx, OFFSET_LOADER
	mov		ax, cx
	
.KEEP_LOADING:
	mov		cl, 1
	call	read_sector

	; 每读一个扇区，就显示一个 "."
	push	ax

	mov		ah, 0eh
	mov		al, '.'
	mov		bl, 0fh
	int  	10h  

	pop		ax

	; 这里要判断一下是否还需要继续读
	pop 	ax
	call	get_next_sector_no
	cmp		ax, 0fffh
	jz		.FILE_LOADED

	; loader.bin 还没加载完，则继续加载
	push	ax
	mov		dx, SECTORS_OF_ROOT_DIR
	add		ax, dx
	add 	ax, CONSTANT_SECTOR_17
	add		bx, [BPB_BytsPerSec]
	jmp		.KEEP_LOADING

.FILE_LOADED:
	jmp		BASE_LOADER:OFFSET_LOADER

.KEEP_GOING_ON:
	inc		di
	jmp		.COMPARE_LOADER_NAME

.ON_LOADER_NAME_DIFFERENT:
	; 比较 loader.bin 名字过程中，如果发现不一致，则先重置 di 和 si，然后在比较下一个32B的根目录项目
	and		di,	0ffe0h
	add		di,	20h
	mov		si,	LOADER_NAME
	jmp		.COMPARE_NEXT_ROOT_ENTRY

.LOADER_NOT_FOUND:
	; 这个地方显示一个 "Loader not found"，然后死循环
	mov		ax, 0
	mov		bx, MSG_LOADER_NOT_FOUND
	mov 	cx, LENGTH_LOADER_NOT_FOUND
	mov		dx, 1300h
	call 	print_message
	jmp $

; SECTOR_NO 位置存放要读取的扇区号
SECTOR_NO	dw	0
REST_SECTORS	dw  SECTORS_OF_ROOT_DIR

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
; 函数：restore_soft_driver
; 描述：复位软驱
;------------------------------------------------------------------------------------------
restore_soft_driver:
	push	ax
	push	dx
	
	xor		ah, ah
	xor		dl, dl
	int 13h

	pop 	dx
	pop		ax

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
	mov		ax, BASE_LOADER
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
; 			数据区
;------------------------------------------------------------------------------------------
; 引导模块名称
; TODO 奇怪为什么后面要加一个 0
LOADER_NAME			db	"LOADER  BIN", 0

IF_ODD				db  0

MSG_LOADER_FOUND:
	db	"Booting"
LENGTH_LOADER_FOUND	equ	$ - MSG_LOADER_FOUND

MSG_LOADER_NOT_FOUND:
	db	"Loader NOT FOUND"
LENGTH_LOADER_NOT_FOUND	equ	$ - MSG_LOADER_NOT_FOUND

WELCOME_MSG:
    db  "Long live China"
LENGTH_WELCOME equ $ - WELCOME_MSG

    times   510-($-$$)  db  0
    dw  0xaa55