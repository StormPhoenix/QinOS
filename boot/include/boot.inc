; ------------------------------------------------
; boot.inc 定义了整个引导过程使用到到常量
; ------------------------------------------------

; OS内核存放的基地址：偏移地址
; TODO 这里提一个问题，如果OS过大，放在这里会不会覆盖掉 loader.bin 本身
BASE_KERNEL		equ 08000h
OFFSET_KERNEL	equ 0100h
KERNEL_ENTRY_POINT      equ 0x30400

; loader.bin 位置
BASE_LOADER     equ 09000h
OFFSET_LOADER   equ 0100h

; TODO 参考一下内存分布表
; 页目录表存放基地址 0M（注意4K对齐）
BASE_PAGE_DIR   equ 0000000h
; 页表存放基本地址 0M + 4K（注意4K对齐）
BASE_PAGE_TABLE equ 0001000h