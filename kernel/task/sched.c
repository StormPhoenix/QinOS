#include "asm/system.h"
#include "pm.h"
#include "string.h"
#include "task.h"
#include "sched.h"
#include "sys_call.h"
#include "mm/mm.h"

extern Task *task_table[];
extern Task *current_task;

// 初始任务数据结构
typedef union task_union {
    Task task;
    char stack[PAGE_SIZE];
} TaskUnion;

// OS 中第一个进程
TaskUnion init_task = {INIT_TASK};


// TODO 暂时缓存三个进程Task结构
TaskUnion test_task_union[NR_TASK];
// TODO 暂时缓存三个进程的执行函数
task_function task_eip[NR_TASK] = {0, TestA, TestC, TestB};
// TODO 三个进程的堆栈
#define USER_STACK_SIZE     1024
char stack_a[USER_STACK_SIZE];
char stack_b[USER_STACK_SIZE];
char stack_c[USER_STACK_SIZE];
char *task_stack[NR_TASK] = {0, stack_a, stack_b, stack_c};

void sched_init() {
    // ldt
    memory_copy(&gdt[SELECTOR_KERNEL_CODE >> 3], sizeof(Descriptor), &(init_task.task.LDT[1]));
    init_task.task.LDT[1].attributes = CODE_ATTR386 | (DPL_USER << 5);
    memory_copy(&gdt[SELECTOR_KERNEL_DATA >> 3], sizeof(Descriptor), &(init_task.task.LDT[2]));
    init_task.task.LDT[2].attributes = DATA_ATTR386_RW | (DPL_USER << 5);

    task_table[0] = (Task *) &init_task;
    // tss entry
    set_tss_desc(FIRST_TSS_ENTRY,
                 virtual_to_linear_addr(SELECTOR_KERNEL_DATA, (u32) &(init_task.task.tss)));
    // ldt entry
    set_gdt_desc(FIRST_LDT_ENTRY,
                 virtual_to_linear_addr(SELECTOR_KERNEL_DATA, (u32) &(init_task.task.LDT)),
                 sizeof(init_task.task.LDT) - 1,
                 SYS_ATTR386_LDT);
    // TODO io_base
    init_task.task.tss.io_base = sizeof(init_task.task.tss);
    // 加载任务 0
    ltr(0);
    lldt(0);

    Task *task;
    // --------- 下面是测试进程 -----------
    for (int i = 1; i < NR_TASK; i++) {
        task = (Task *) &test_task_union[i];
        task_table[i] = task;
        memory_set((void *) (&(task->tss)), (char) 0, sizeof(task->tss));

        // 初始化段寄存器
        task->tss.cs = (8 & SELECTOR_TI_MASK & SELECTOR_RPL_MASK) | SELECTOR_TI_LDT | RPL_USER;
        task->tss.ds = (16 & SELECTOR_TI_MASK & SELECTOR_RPL_MASK) | SELECTOR_TI_LDT | RPL_USER;
        task->tss.es = (16 & SELECTOR_TI_MASK & SELECTOR_RPL_MASK) | SELECTOR_TI_LDT | RPL_USER;
        task->tss.fs = (16 & SELECTOR_TI_MASK & SELECTOR_RPL_MASK) | SELECTOR_TI_LDT | RPL_USER;
        task->tss.gs = (SELECTOR_VIDEO & SELECTOR_TI_MASK & SELECTOR_RPL_MASK) | SELECTOR_TI_GDT | RPL_USER;
        task->tss.eip = (u32) task_eip[i];
        task->tss.io_base = sizeof(task->tss);
        // 初始化堆栈、EFLAGS
        task->tss.ss = (16 & SELECTOR_TI_MASK & SELECTOR_RPL_MASK) | SELECTOR_TI_LDT | RPL_USER;
        task->tss.esp = (u32) task_stack[i] + USER_STACK_SIZE;
        task->tss.ss0 = SELECTOR_KERNEL_DATA;
        task->tss.esp0 = (u32) task + PAGE_SIZE;
        task->tss.eflags = 0x0202;
        // 初始化 gdt LDT entry、LDT selector
        set_ldt_desc(
                FIRST_LDT_ENTRY + i * 2,
                virtual_to_linear_addr(SELECTOR_KERNEL_DATA, (u32) &(task->LDT)),
                sizeof(task->LDT) - 1);

        task->tss.ldt = _LDT(i);
        // 初始化 LDT
        memory_copy(&gdt[SELECTOR_KERNEL_CODE >> 3], sizeof(Descriptor), &(task->LDT[1]));
        task->LDT[1].attributes = CODE_ATTR386 | (DPL_USER << 5);
        memory_copy(&gdt[SELECTOR_KERNEL_DATA >> 3], sizeof(Descriptor), &(task->LDT[2]));
        task->LDT[2].attributes = DATA_ATTR386_RW | (DPL_USER << 5);

        // 初始化 gdt TSS entry
        set_tss_desc(
                FIRST_TSS_ENTRY + i * 2,
                virtual_to_linear_addr(SELECTOR_KERNEL_DATA, (u32) &(task->tss)));
    }
    current_task = (Task *) &init_task;

    // 设置系统调用
    set_system_gate(INT_VECTOR_SYS_CALL, system_call);
}


void schedule() {
    static int task_no = 1;
    task_no++;
    if (task_no >= NR_TASK) {
        task_no = 0;
    }
    switch_to(task_no);
}

int sys_get_ticks() {
    print_string("get ticks");
    return 0;
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA() {
    int i = 0;
    while (1) {
        print_string("A");
        print_string(".");
        int ret_code = get_ticks();
        print_hex(ret_code);
        delay();
    }
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB() {
    int i = 0x1000;
    while (1) {
        print_string("B");
        print_string(".");
        delay();
    }
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestC() {
    int i = 0x2000;
    while (1) {
        print_string("C");
        print_string(".");
        delay();
    }
}

void delay() {
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 1000; j++) {

        }
    }
}