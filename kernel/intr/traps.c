/**
 * kernel/trap.c
 * 
 * trap.c 包括了所有的中断处理函数的声明。这些函数的地址会被填写到 idt 表中。
 * 这些中断处理函数的具体实现在 kernel.asm 里面，每个实现又回反过来回调 trap.c 
 * 里的实现函数
 */

#include "asm/system.h"
#include "bits.h"
#include "pm.h"
#include "sched.h"
#include "string.h"
#include "traps.h"
#include "intr.h"
#include "type.h"


// -------- i386 中断处理函数 --------
/**
 * 举个例子，这儿的 divide_error 的具体实现是 kernel/kernel.asm 里
 * 面的 divide_error，而 divide_error 又会调用 trap.c 中的 do_divide_error，
 * 这样就是绕了一个圈子，trap.c -> kernel.asm -> trap.c，这样做的目的是
 * 在 kernel.asm 里面可以将 error_code 和 中断向量 vector 压入堆栈，这样就有五个
 * 参数，但是这个在c语言里面做不到
 */
// -------- 内部中断处理函数 --------
void divide_error(void);

void debug(void);

void nmi(void);

void breakpoint(void);

void overflow(void);

void bounds(void);

void undefine_opcode(void);

void device_not_available(void);

void double_fault(void);

void coprocessor_segment_overrun(void);

void invalid_tss(void);

void segment_not_present(void);

void stack_error(void);

void general_protection(void);

void page_fault(void);

void reserved(void);

void coprocessor_error(void);

void alignment_check(void);

// 内部中断统一交由 exception 来处理
void exception(void);

// -------- 外部中断处理函数 --------

// 时钟中断
void irq0(void);

void irq1(void);

void irq2(void);

void irq3(void);

void irq4(void);

void irq5(void);

void irq6(void);

void irq7(void);

void irq8(void);

void irq9(void);

void irq10(void);

void irq11(void);

void irq12(void);

void irq13(void);

void irq14(void);

void irq15(void);


void do_divide_error(int int_vector, int error_code, int eip, int cs, int eflags) {
    // TODO
}


void do_timer(long dpl) {
    schedule();
}


/**
 * hardware_interrupt_handler
 * 
 * 参数：
 *  int_vector  中断向量号
 * 描述：默认的外部（硬件）中断处理函数
 */
void hardware_interrupt_handler(int int_vector) {
    // TODO 默认处理，以后还要修改的
    print_string("Hardware intr occur\n");
}


/**
 * exception_handler
 * 
 * 参数：
 *  int_vector  中断向量号
 *  error_code  错误码
 *  cs:eip      返回地址
 *  eflags      原标志eflag
 * 
 * 描述：默认的内部中断处理函数
 */
void exception_handler(int int_vector, unsigned int error_code, int eip, int cs, int eflags) {
    char *err_msg[] = {"#DE Divide Error",
                       "#DB RESERVED",
                       "#NMI Interrupt",
                       "#BP Breakpoint",
                       "#OF Overflow",
                       "#BR BOUND Range Exceeded",
                       "#UD Invalid Opcode (Undefined Opcode)",
                       "#NM Device Not Available (No Math Coprocessor)",
                       "#DF Double Fault",
                       "    Coprocessor Segment Overrun (reserved)",
                       "#TS Invalid TSS",
                       "#NP Segment Not Present",
                       "#SS Stack-Segment Fault",
                       "#GP General Protection",
                       "#PF Page Fault",
                       "--  (Intel reserved. Do not use.)",
                       "#MF x87 FPU Floating-Point Error (Math Fault)",
                       "#AC Alignment Check",
                       "#MC Machine Check",
                       "#XF SIMD Floating-Point Exception"
    };
    print_string(err_msg[int_vector]);
    // TODO 清楚冗余代码
    print_string(" ");
    print_hex(error_code);
    print_string(" ");
    print_hex(eip);
    print_string(" ");
    print_hex(cs);
    print_string(" ");
    print_hex(eflags);
}



/** 设置中断中断控制芯片 */
PRIVATE void setup_8259A() {
    // 初始化8259A芯片
    // Master 8259，ICW1
    out_byte(MASTER_8259_CTL, 0x11);
    // Slave 8259， ICW1
    out_byte(SLAVE_8259_CTL, 0x11);
    // Master 8259， ICW2
    out_byte(MASTER_8259_MASK, 0x20);
    // Slave 8259， ICW2
    out_byte(SLAVE_8259_MASK, 0x28);
    // Master 8259， ICW3
    out_byte(MASTER_8259_MASK, 0x4);
    // Slave 8259， ICW3
    out_byte(SLAVE_8259_MASK, 0x2);
    // Master 8259， ICW4
    out_byte(MASTER_8259_MASK, 0x1);
    // Slave 8259， ICW4
    out_byte(SLAVE_8259_MASK, 0x1);

    // -------- 默认屏蔽所有中断 -----------
    out_byte(MASTER_8259_MASK, 0xff);
    out_byte(SLAVE_8259_MASK, 0xff);
}


// 设置IDT表
void trap_init() {
    setup_8259A();
    // 设置外部中断
    set_intr_gate(INT_VECTOR_IRQ0 + 0, timer_interrupt);
    set_intr_gate(INT_VECTOR_IRQ0 + 1, irq1);
    set_intr_gate(INT_VECTOR_IRQ0 + 2, irq2);
    set_intr_gate(INT_VECTOR_IRQ0 + 3, irq3);
    set_intr_gate(INT_VECTOR_IRQ0 + 4, irq4);
    set_intr_gate(INT_VECTOR_IRQ0 + 5, irq5);
    set_intr_gate(INT_VECTOR_IRQ0 + 6, irq6);
    set_intr_gate(INT_VECTOR_IRQ0 + 7, irq7);
    set_intr_gate(INT_VECTOR_IRQ0 + 8, irq8);
    set_intr_gate(INT_VECTOR_IRQ0 + 9, irq9);
    set_intr_gate(INT_VECTOR_IRQ0 + 10, irq10);
    set_intr_gate(INT_VECTOR_IRQ0 + 11, irq11);
    set_intr_gate(INT_VECTOR_IRQ0 + 12, irq12);
    set_intr_gate(INT_VECTOR_IRQ0 + 13, irq13);
    set_intr_gate(INT_VECTOR_IRQ0 + 14, irq14);
    set_intr_gate(INT_VECTOR_IRQ0 + 15, irq15);

    // 设置内部中断
    set_intr_gate(INT_VECTOR_DIVIDE_ERROR, divide_error);
    set_intr_gate(INT_VECTOR_DEBUG, debug);
    set_intr_gate(INT_VECTOR_NMI, nmi);
    set_intr_gate(INT_VECTOR_BREAKPOINT, breakpoint);
    set_intr_gate(INT_VECTOR_OVERFLOW, overflow);
    set_intr_gate(INT_VECTOR_BOUNDS, bounds);
    set_intr_gate(INT_VECTOR_UD, undefine_opcode);
    set_intr_gate(INT_VECTOR_DEV_NOT_AVAILABLE, device_not_available);
    set_intr_gate(INT_VECTOR_DOUBLE_FAULT, double_fault);
    set_intr_gate(INT_VECTOR_COPR_SEG_OVERRUN, coprocessor_segment_overrun);
    set_intr_gate(INT_VECTOR_INVALID_TSS, invalid_tss);
    set_intr_gate(INT_VECTOR_SEG_NOT_PRESENT, segment_not_present);
    set_intr_gate(INT_VECTOR_STACK_ERROR, stack_error);
    set_intr_gate(INT_VECTOR_GENERAL_PROTECTION, general_protection);
    set_intr_gate(INT_VECTOR_PAGE_FAULT, page_fault);
    set_intr_gate(INT_VECTOR_RESERVED, reserved);
    set_intr_gate(INT_VECTOR_COPR_ERROR, coprocessor_error);
    set_intr_gate(INT_VECTOR_ALIGN_CHECK, alignment_check);


    // set idt_ptr
    *((u16 *) (&idt_ptr[0])) = IDT_SIZE * sizeof(Gate) - 1;
    (*(u32 *) (&idt_ptr[2])) = (u32) &idt;
    lidt(idt_ptr);
    print_string("Finish setting the bitch idt!\n");
}