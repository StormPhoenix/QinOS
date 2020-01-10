    org     07c00h
    mov     ax, cs
    mov     es, ax
    call    display_string
    jmp     $

display_string:
    mov     ax, message
    mov     bp, ax
    mov     cx, 20
    mov     ax, 1301h
    mov     bx, 000ch                       
    mov     dx, 1200h                       ; DH, DL = 起始行列
    ;mov     al, 3
    ;mov     dx, 0
    int     10h
    ret
message:
    db      "Hello, Storm Phoenix"
    times   510 - ($ - $$)      db  0
    dw      0xaa55