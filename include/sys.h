#ifndef _SYS_H_
#define _SYS_H_

#include "type.h"


// 代码段、数据段描述符
typedef struct descriptor
{
    // 段限长，0 - 15 位
    u16 limit_low;
    // 基地址，16 - 31 位
    u16 base_low;
    // 基地址，32 - 39 位
    u8 base_mid;
    // 属性1，40 - 47
    u8 attributes;
    // 4 位段限长 + 4 位属性，48 - 55 位
    u8 limit_high_and_attributes;
    // 基地址，56 - 63 位
    u8 base_high;
} Descriptor;


// 中断向量号
#define INT_VECTOR_DIVIDE_ERROR         0x0
#define INT_VECTOR_DEBUG                0x1
#define INT_VECTOR_NMI                  0x2
#define INT_VECTOR_BREAKPOINT           0x3
#define INT_VECTOR_OVERFLOW             0x4
#define INT_VECTOR_BOUNDS               0x5
#define INT_VECTOR_UD                   0x6
#define INT_VECTOR_DEV_NOT_AVAILABLE    0x7
#define INT_VECTOR_DOUBLE_FAULT         0x8
#define INT_VECTOR_COPR_SEG_OVERRUN     0x9
#define INT_VECTOR_INVALID_TSS          0x10
#define INT_VECTOR_SEG_NOT_PRESENT      0x11
#define INT_VECTOR_STACK_ERROR          0x12
#define INT_VECTOR_GENERAL_PROTECTION   0x13
#define INT_VECTOR_PAGE_FAULT           0x14
#define INT_VECTOR_RESERVED             0x15
#define INT_VECTOR_COPR_ERROR           0x16
#define INT_VECTOR_ALIGN_CHECK          0x17


// 外部中断向量
#define INT_VECTOR_IRQ0                 0x20
#define INT_VECTOR_IRT8                 0x28

// 内核代码段选择符
#define SELECTOR_KERNEL_CODE 0x8

// 门类型
#define CALL_GATE           12 
/*
 * IDT 表只有三个类型的 Gate，
 * Trap Gate 和 Interrupt Gate 的区别是对 EFLAGS 中的 IF 标志。
 * IF 标志用来屏蔽其他中断的。这意味这 Trap Gate 会被打断，而 Interrupt Gate 不会
 */
#define TRAP_GATE           (u8)15
#define INTERRUPT_GATE      (u8)14
#define TASK_GATE           (u8)5

#define DPL_KERNEL          (unsigned char) 0

// 调用门描述符
typedef struct gate
{
    // 段中偏移值低16位
    u16 offset_low;
    // 段选择符
    u16 selector;
    // 参数个数
    u8 param_count;
    // type、dpl、p
    u8 attributs;
    // 段中偏移值高16位
    u16 offset_high;
} Gate;

#endif