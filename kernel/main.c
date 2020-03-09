/*
* 内核起始文件
*/

#include "bits.h"
#include "protect.h"
#include "sched.h"
#include "string.h"
#include "sys.h"
#include "traps.h"
#include "traps.h"



// 主函数入口
void kernel_main() {
    setup_gdt();
    trap_init();
    sti();
    // 初始化 OS 第一个进程
    setup_paging();
    // 初始化调度
    sched_init();
    // 开启时钟中断
    enable_irq(IRQ_CLOCK);
    // 跳入用户态，Task main() 将成为 OS 的第一个 Task
    print_string("enter user mode\n");
    move_to_user_mode();
    while (1) {
        print_string("main task\n");
        delay();
    }
}
