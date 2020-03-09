global  out_byte
global  in_byte
global  enable_irq

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


; ------------------------------------------------
; enable_irq
; 参数：
;   irq    外部中断号，[esp + 4]
; 描述：对外部中断号irq取消屏蔽
; ------------------------------------------------
enable_irq:
        ; irq
        mov     ecx, [esp + 4]
        pushf
        cli
        mov     ah, ~1
        ; ah = ~(1 << (irq % 8))
        rol     ah, cl
        cmp     cl, 8
        ; enable irq >= 8 at the slave 8259
        jae     enable_8
enable_0:
        in      al, 0x21
        and     al, ah
        ; clear bit at master 8259
        out     0x21, al
        popf
        ret
enable_8:
        in      al, 0xA1
        and     al, ah
        ; clear bit at slave 8259
        out     0xA1, al
        popf
        ret