#include "bits.h"
#include "console.h"
#include "intr.h"

#include "string.h"


/** 键盘中断 */
void do_keyboard() {
    u8 code = in_byte(0x60);
    // 入队
    int next = (keyboard_buffer.head + 1) % keyboard_buffer.size;
//    print_hex(keyboard_buffer.size);
//    print_hex(next);
    if (next != keyboard_buffer.tail) {
        // TODO 这里的code应该转化一下吧
        keyboard_buffer.buffer[next] = code;
        keyboard_buffer.head = next;
    } else {
        print_string("keyboard buffer is full.\n");
    }
    print_hex(code);
}

void keyboard_init() {
    // 初始化缓冲区
    keyboard_buffer.head = 1;
    keyboard_buffer.tail = 0;
    keyboard_buffer.size = 5;
    enable_irq(IRQ_KEYBOARD);
}