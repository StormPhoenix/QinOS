/*
* 内核起始文件
*/

#include "type.h"
#include "protect.h"
#include "string.h"
#include "bits.h"
#include "sched.h"
#include "traps.h"
#include "task.h"


// 主函数入口
void kernel_main() {
    // 初始化 OS 第一个进程
    setup_paging();
    // 初始化调度
    initialize_schedule();
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
