#ifndef _HEAD_H_
#define _HEAD_H_


/** 内部中断向量 */
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
#define INT_VECTOR_INVALID_TSS          0xA
#define INT_VECTOR_SEG_NOT_PRESENT      0xB
#define INT_VECTOR_STACK_ERROR          0xC
#define INT_VECTOR_GENERAL_PROTECTION   0xD
#define INT_VECTOR_PAGE_FAULT           0xE
#define INT_VECTOR_RESERVED             0xF
#define INT_VECTOR_COPR_ERROR           0x10
#define INT_VECTOR_ALIGN_CHECK          0x11

/** 外部中断向量号 */
#define INT_VECTOR_IRQ0                 0x20
#define INT_VECTOR_IRT8                 0x28

/** 系统调用中断 */
#define INT_VECTOR_SYS_CALL             0x70

/** 外部中断向量在 8259A 上面的引脚 */
#define IRQ_CLOCK                       0
#define IRQ_KEYBOARD                    1


/** 8295A 芯片的 in、out 端口 */
#define MASTER_8259_CTL 0x20
#define MASTER_8259_MASK 0x21
#define SLAVE_8259_CTL 0xa0
#define SLAVE_8259_MASK 0xa1


/** EOI 中断处理完成标志 */
#define EOI 0x20


/**
 * 门类型
 * IDT 表只有三个类型的 Gate，
 * Trap Gate 和 Interrupt Gate 的区别是对 EFLAGS 中的 IF 标志。
 * IF 标志用来屏蔽其他中断的。这意味这 Trap Gate 会被打断，而 Interrupt Gate 不会
 */
#define CALL_GATE           (u8)12
#define TRAP_GATE           (u8)15
#define INTER_GATE          (u8)14
#define TASK_GATE           (u8)5


#endif