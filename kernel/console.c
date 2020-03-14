#include "asm/system.h"
#include "bits.h"
#include "console.h"
#include "intr.h"
#include "string.h"
#include "thread.h"
#include "type.h"

static Mutex key_buff_mutex = {1};

/** 键盘中断 */
void do_keyboard() {
    u8 code = in_byte(0x60);
    request_lock(&key_buff_mutex);
    // 入队
    int next = (keyboard_buffer.head + 1) % keyboard_buffer.size;
    if (next != keyboard_buffer.tail) {
        keyboard_buffer.buffer[keyboard_buffer.head] = code;
        keyboard_buffer.head = next;
    } else {
        print_string("keyboard buffer is full.\n");
    }
    release_lock(&key_buff_mutex);
}


/** 初始化 keyboard */
void keyboard_init() {
    // 初始化缓冲区
    keyboard_buffer.head = 1;
    keyboard_buffer.tail = 0;
    keyboard_buffer.size = 5;
    enable_irq(IRQ_KEYBOARD);
}


/** 读取 keyboard 缓冲 */
static void read_key_buffer() {
    u8 scan_code;
    // TODO 在多线程情况下，这段代码会出错，buffer 的操作必须上锁
// TODO   cli(); 用户态好像不能执行这个指令
    int next = (keyboard_buffer.tail + 1) % keyboard_buffer.size;
    if (next != keyboard_buffer.head) {
        scan_code = keyboard_buffer.buffer[next];
        keyboard_buffer.tail = next;
        print_hex(scan_code);
//        if (scan_code == ) {
//
//        }
//        print_hex(scan_code);
    }
//    sti();
}


// TODO tty，测试放这儿，之后移动到其他地方

void tty_task() {
    while (True) {
        request_lock(&key_buff_mutex);
        read_key_buffer();
        release_lock(&key_buff_mutex);
    }
}