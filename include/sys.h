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


/** 调入内核态 */
#define move_to_user_mode() \
    __asm__( \
        "movl   %%esp, %%eax\n\t" \
        "pushl  $0x17\n\t" \
        "pushl  %%eax\n\t" \
        "pushfl\n\t" \
        "pushl  $0x0f\n\t" \
        "pushl  $1f\n\t" \
        "iret\n" \
        "1:\n\t" \
        "movl   $0x17, %%eax\n\t" \
        "movl   %%eax, %%ds\n\t" \
        "movl   %%eax, %%es\n\t" \
        "movl   %%eax, %%fs\n\t" \
        "movl   $0x1b, %%eax\n\t" \
        "movl   %%eax, %%gs\n\t" \
        ::: "ax" \
    )


#endif