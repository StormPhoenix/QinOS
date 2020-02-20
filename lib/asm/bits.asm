global  out_byte
global  in_byte

[SECTION .text]

; ------------------------------------------------
; out_byte
; 参数：
;   port    端口，[esp + 4]
;   byte    字节，[esp + 8]
; 描述：out_byte
; ------------------------------------------------
out_byte:
    push    ebp
    mov     ebp, esp
    push    edx

    xor     edx, edx
    ; 端口
    mov     edx, [ebp + 8]
    ; 值
    mov     al, [ebp + 12]
    ; 写入
    out     dx, al
    nop
    nop

    pop     edx
    pop     ebp
    ret


; ------------------------------------------------
; in_byte
; 参数：
;   port    端口，[esp + 4]
; 描述：读入参数，返回值为 eax
; ------------------------------------------------
in_byte:
    push    ebp
    mov     ebp, esp
    push    edx

    xor     edx, edx
    ; 端口
    mov     edx, [ebp + 8]
    ; 读入
    in      al, dx
    nop
    nop

    pop     edx
    pop     ebp
    ret