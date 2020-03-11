#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_



/** 硬件中断处理函数 */
// 时钟中断
void timer_interrupt(void);

/**
 * 时钟中断处理函数
 * @param cpl 被中断程序的特权级
 */
void do_timer(long cpl);

/**
 * 设置 idt 表
 */
void trap_init();

#endif