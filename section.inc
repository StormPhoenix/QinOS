; 特权级
; 特权级0
DPL0            equ 00h
; 特权级1
DPL1            equ 20h
; 特权级2
DPL2            equ 40h
; 特权级3
DPL3            equ 60h

; 数据段
; 只读数据段，已存在P=1，特权级DPL=0，高位扩E=0，不可写W=0，未访问A=0
DATA_R          equ 90h                    
; 只读数据段，已存在P=1，特权级DPL=0，高位扩E=0，可写W=1，未访问A=0
DATA_RW         equ 92h    
; 只读数据段，已存在P=1，特权级DPL=0，高位扩E=0，可写W=1，已访问A=1
DATA_RWA        equ 93h                    

; 代码段
; 32 位代码段，D=1
CODE_32         equ 4000h                  
; 代码段，段限长4KB
CODE_LIMIT_G    equ 8000h                  

; 代码段，已存在P=1，特权级DPL=0，非一致代码段C=0，不可读R=0，未访问A=0
CODE_C          equ 98h                    
; 代码段，已存在P=1，特权级DPL=0，非一致代码段C=0，可读R=1，未访问A=0
CODE_CR         equ 9ah                    
; 代码段，已存在P=1，特权级DPL=0，一致代码段C=1，不可读R=0，未访问A=0
CODE_C0         equ 9ch                    
; 代码段，已存在P=1，特权级DPL=0，一致代码段C=1，可读R=1，未访问A=0
CODE_C0R        equ 9eh                    

; GDT 描述符宏
; params: 
;         Limit: 低 20 位有效
;         Base: 32位ÍÍ
;         Attr: 

%macro GDT_DESCRIPTOR 3
    dw  %1 & 0ffffh
    dw  %2 & 0ffffh
    db  (%2 >> 16) & 0ffh
    dw  ((%1 >> 8) & 0f00h) | (%3 & 0f0ffh)          
    db  (%2 >> 24) & 0ffh
%endmacro