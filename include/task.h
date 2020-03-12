#ifndef _TASK_H_
#define _TASK_H_

#include "type.h"
#include "pm.h"


typedef struct task_state_segment {
    // 高 30 bits 为 0 
    u32 back_link;
    u32 esp0;
    // 高 30 bits 为 0 
    u32 ss0;
    u32 esp1;
    // 高 30 bits 为 0 
    u32 ss1;
    u32 esp2;
    // 高 30 bits 为 0 
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    // ldt 低 16 位有效
    u32 ldt;
    u16 trap;
    u16 io_base;
} TSS;


typedef struct task_struct {
    TSS tss;
    Descriptor LDT[MAX_LDT_SIZE];
    /* 进程id */
    long task_id;
    /* 开始运行时间 */
    long start_time;
    /* 优先级 */
    long priority;
    /* 剩余时间片 */
    long counter;
} Task;


#endif