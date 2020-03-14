#ifndef _CONSOLE_H_
#define _CONSOLE_H_


#define BREAK_FLAG      0x0080


/** 键盘缓冲结构，一个循环队列结构 */
typedef struct key_buffer {
    // 队列中下一条空位的 index
    int head;
    // 队尾 index
    int tail;
    // 缓冲区大小
    int size;
    // 键盘缓冲
    char buffer[128];
} KeyBuffer;


/** 中断任务 */
void tty_task();


/** 键盘输入缓冲 */
KeyBuffer keyboard_buffer;


/** 键盘初始化 */
void keyboard_init();


#endif