#ifndef _SYS_H_
#define _SYS_H_

/** 开启中断指令 */
#define sti() { \
    __asm__(\
        "sti\n\t" \
    );\
}

/** 保存gdt地址 */
#define sgdt(ptr) { \
    __asm__( \
        "sgdt   %0 \n\t" \
        ::"m" (ptr) \
    ); \
}


/** 加载 gdt */
#define lgdt(ptr) { \
    __asm__( \
        "lgdt   %0 \n\t" \
        ::"m" (ptr) \
    ); \
}


/** 加载 idt */
#define lidt(ptr) { \
    __asm__( \
        "lidt   %0 \n\t" \
        ::"m" (ptr) \
    ); \
}


/** 加载 tss */
#define ltr(n) {\
    __asm__( \
        "ltr    %%ax \n\t" \
        :: "a" (_TSS(n)) \
    ); \
}


/** 加载 ldt */
#define lldt(n) {\
    __asm__( \
        "lldt   %%ax \n\t" \
        :: "a" (_LDT(n)) \
    ); \
}


#endif