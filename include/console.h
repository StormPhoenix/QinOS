#ifndef _CONSOLE_H_
#define _CONSOLE_H_

/** 回调函数，用于处理键盘缓冲中读出的数据 */
void terminal_key_callback(u32 key, int tty_no);


/** 中断任务 */
void tty();


/** 键盘初始化 */
void keyboard_init();


#endif