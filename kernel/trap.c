/*
 * kernel/trap.c
 * 
 * trap.c 包括了所有的中断处理函数的声明。这些函数的地址会被填写到 idt 表中。
 * 这些中断处理函数的具体实现在 kernel.asm 里面，每个实现又回反过来回调 trap.c 
 * 里的实现函数
 */

#include "sys.h"
#include "const.h"
#include "type.h"

// IDT
Gate idt[IDT_SIZE];
// IDT 位置值
u8 idt_ptr[6];

// 端口读写声明 bits.asm
extern void out_byte(u16 port, u8 byte);
// 打印语句
extern void print_string(char *msg);

// -------- i386 中断处理函数 -------- 
/*
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


// 中断处理函数的具体实现
void do_divide_error(int int_vector, int error_code, int eip, int cs, int eflags) 
{
    // TODO
}


/*
 * hardware_interrupt_handler
 * 
 * 参数：
 *  int_vector  中断向量号
 * 描述：默认的外部（硬件）中断处理函数
 */
void hardware_interrupt_handler(int int_vector) {
    // TODO 默认处理，以后还要修改的
    print_string("Hardware interrupt occur\n");
}


/*
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
void exception_handler(int int_vector, int error_code, int eip, int cs, int eflags) 
{
    char * err_msg[] = {"#DE Divide Error",
			    "#DB RESERVED",
			    "--  NMI Interrupt",
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
    print_string("\n");
}


/*
 * 初始化 idt 表项
 * param:
 *  vector 中断向量号
 *  type   门类型
 *  dpl    等级
 */
PRIVATE void set_idt_gate(unsigned char vector, u8 type, int_handler handler, unsigned char dpl) {
    Gate *gate_entry = &idt[vector];
    // 偏移地址
    u32 offset = (u32) handler;
    gate_entry->offset_low = offset & 0x0000ffff;
    gate_entry->selector = SELECTOR_KERNEL_CODE;
    // TODO param_count 不可能全是0
    gate_entry->param_count = 0;
    gate_entry->attributs = (type & 0x0f) | ((dpl << 5) & 0x60) | 0x80;
    gate_entry->offset_high = (offset >> 16) & 0x0ffff;
}


// 设置中断中断控制芯片
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

    // -------- 屏蔽其他所有中断
    out_byte(MASTER_8259_MASK, 0xff);
    out_byte(SLAVE_8259_MASK, 0xff);
}


// 设置IDT表
void init_idt() {
    setup_8259A();
    // 设置外部中断
    set_idt_gate(INT_VECTOR_IRQ0 + 0, INTERRUPT_GATE, irq0, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 1, INTERRUPT_GATE, irq1, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 2, INTERRUPT_GATE, irq2, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 3, INTERRUPT_GATE, irq3, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 4, INTERRUPT_GATE, irq4, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 5, INTERRUPT_GATE, irq5, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 6, INTERRUPT_GATE, irq6, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 7, INTERRUPT_GATE, irq7, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 8, INTERRUPT_GATE, irq8, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 9, INTERRUPT_GATE, irq9, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 10, INTERRUPT_GATE, irq10, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 11, INTERRUPT_GATE, irq11, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 12, INTERRUPT_GATE, irq12, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 13, INTERRUPT_GATE, irq13, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 14, INTERRUPT_GATE, irq14, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_IRQ0 + 15, INTERRUPT_GATE, irq15, DPL_KERNEL);

    // 设置内部中断
    set_idt_gate(INT_VECTOR_DIVIDE_ERROR, INTERRUPT_GATE, divide_error, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_DEBUG, INTERRUPT_GATE, debug, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_NMI, INTERRUPT_GATE, nmi, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_BREAKPOINT, INTERRUPT_GATE, breakpoint, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_OVERFLOW, INTERRUPT_GATE, overflow, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_BOUNDS, INTERRUPT_GATE, bounds, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_UD, INTERRUPT_GATE, undefine_opcode, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_DEV_NOT_AVAILABLE, INTERRUPT_GATE, device_not_available, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_DOUBLE_FAULT, INTERRUPT_GATE, double_fault, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_COPR_SEG_OVERRUN, INTERRUPT_GATE, coprocessor_segment_overrun, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_INVALID_TSS, INTERRUPT_GATE, invalid_tss, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_SEG_NOT_PRESENT, INTERRUPT_GATE, segment_not_present, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_STACK_ERROR, INTERRUPT_GATE, stack_error, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_GENERAL_PROTECTION, INTERRUPT_GATE, general_protection, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_PAGE_FAULT, INTERRUPT_GATE, page_fault, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_RESERVED, INTERRUPT_GATE, reserved, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_COPR_ERROR, INTERRUPT_GATE, coprocessor_error, DPL_KERNEL);
    set_idt_gate(INT_VECTOR_ALIGN_CHECK, INTERRUPT_GATE, alignment_check, DPL_KERNEL);
    // set idt_ptr
    *((u16*) (&idt_ptr[0])) = IDT_SIZE * sizeof(Gate) -  1;
    (*(u32*) (&idt_ptr[2])) = (u32) &idt;

    print_string("Finish setting the bitch IDT!\n");
}