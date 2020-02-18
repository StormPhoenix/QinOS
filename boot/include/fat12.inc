; 这个地方参考了书中的操作系统，用的是FAT12文件系统，目的是方便向镜像中添加新文件，
; 所以引导镜像的第一个扇区，既是作为引导扇区，也是作为操作系统可识别的FAT12文件系统
; 也方便程序运行过程中读取文件，所以下面读取 loader 模块的代码需要参考FAT12文件系统
; 的格式来编写    

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

;--------------------------------------------------------
; 相关常量定义

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
