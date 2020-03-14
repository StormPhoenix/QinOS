/**
 * 定义需要用到汇编的宏
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_


#include "intr.h"
#include "pm.h"


#define __set_gdt_desc__(entry, base, limit, attr) \
    __asm__ ( \
    "movw   %%cx, (%0)\n\t" \
    "movw   %%bx, 2(%0)\n\t" \
    "rorl   $16, %%ebx\n\t" \
    "movb   %%bl, 4(%0)\n\t" \
    "movb   %%dl, 5(%0)\n\t" \
    "rorl   $16, %%ecx\n\t" \
    "andl   $0x0f, %%ecx\n\t" \
    "rorl   $8, %%edx\n\t" \
    "andl   $0xf0, %%edx\n\t" \
    "orl    %%edx, %%ecx\n\t" \
    "movb   %%cl, 6(%0)\n\t" \
    "movb   %%bh, 7(%0)\n\t" \
    :: "a" (entry), "b" (base), "c" (limit), "d" (attr) \
    ) \


/**
 * __set_gdt_desc__ 的 C 语言形式
void set_gdt_descriptor(Descriptor *descriptor, u32 base, u32 limit, u16 attributes) {
    // 参考 i386 手册设置 descriptor
    descriptor->limit_low = limit & 0xffff;
    descriptor->base_low = base & 0xffff;
    descriptor->base_mid = (base >> 16) & 0xff;
    descriptor->attributes = attributes & 0xff;
    descriptor->limit_high_and_attributes = ((limit >> 16) & 0x0f) | ((attributes >> 8) & 0xf0);
    descriptor->base_high = (base >> 24) & 0x0ff;
}
 */


/** 设置 gdt 描述符号 */
#define set_gdt_desc(n, base, limit, attr) \
    __set_gdt_desc__(&gdt[n], base, limit, attr)


/** 设置 tss 描述符 */
#define set_tss_desc(n, addr) \
    set_gdt_desc(n, addr, 104, SYS_ATTR386_TSS)


/** 设置 ldt 描述符 */
#define set_ldt_desc(n, addr, limit) \
    set_gdt_desc(n, addr, limit, SYS_ATTR386_LDT)


/** 设置中断门 */
#define set_intr_gate(vector, addr) \
    __set_gate__(&idt[vector], INTER_GATE, DPL_KERNEL, addr)


/** 设置陷阱门 */
#define set_trap_gate(vector, addr) \
    __set_gate__(&idt[vector], TRAP_GATE, DPL_KERNEL, addr)


/** 设置系统陷阱门 */
#define set_system_gate(vector, addr) \
    __set_gate__(&idt[vector], TRAP_GATE, DPL_USER, addr)


#define __set_gate__(gate_addr, type, dpl, addr) \
    __asm__( \
        "movw   %%dx, %%ax\n\t" \
        "movw   %0, %%dx\n\t" \
        "movl   %%eax, %1\n\t" \
        "movl   %%edx, %2\n\t" \
        : \
        : "i" ((short) (0x8000 + (dpl << 13) + (type << 8))), \
        "o" (*((char *) (gate_addr))), \
        "o" (*((char *) (gate_addr) + 4)), \
        "d" ((char *) (addr)), "a" (0x00080000) \
        ) \


/** 开启中断指令 */
#define sti() { \
    __asm__(\
        "sti\n\t" \
    );\
}


/** 关闭中断指令 */
#define cli() { \
    __asm__(\
        "cli\n\t" \
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


/** 跳入内核态 */
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


/** 任务切换 */
#define switch_to(n) {\
    struct {long a, b;} __tmp;\
    __asm__(\
        "cmpl %%ecx, current_task\n\t" \
        "je 1f\n\t" \
        "movw %%dx, %1\n\t" \
        "xchgl %%ecx, current_task\n\t" \
        "ljmp *%0 \n\t" \
        "1:" \
        ::"m" (*&__tmp.a), "m" (*&__tmp.b),\
        "d" (_TSS(n)), "c" ((long) task_table[n])\
    );\
}


#endif