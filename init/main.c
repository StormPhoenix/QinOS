/*
* 内核起始文件
*/

#include "asm/system.h"
#include "bits.h"
#include "pm.h"
#include "sched.h"
#include "string.h"
#include "traps.h"


/** 设置页表 */
void setup_paging() {
    // 页面表被放置在内存 0 起始处
    page_dir = 0;
}


/** 设置gdt表 */
void setup_gdt() {
    sgdt(gdt_ptr);
    memory_set(gdt, 0, sizeof(Descriptor) * GDT_SIZE);
    // 将 gdt_ptr 指向的 gdt 赋值给这个文件中的 gdt
    memory_copy(
            (void *) (*((u32 *) (&gdt_ptr[2]))),
            (int) (*((u16 *) (&gdt_ptr[0])) + 1),
            (void *) &gdt
    );

    *((u16 *) (&gdt_ptr[0])) = GDT_SIZE * sizeof(Descriptor) - 1;
    *((u32 *) (&gdt_ptr[2])) = (u32) &gdt;
    lgdt(gdt_ptr);
    print_string("You superise mother-fucker!\n");
}

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
    int i = 1;
    while (1) {
        print_string("main task");
        print_hex(i);
        print_string("\n");
        i ++;
        delay();
    }
}
