/**
 * 定义键盘处理操作，相关宏定义和 keymap
 * 处理流程太繁琐了，处理按键的操作都是从 Orange'S 复制过来的
 */

#include "asm/system.h"
#include "bits.h"
#include "console.h"
#include "global.h"
#include "intr.h"
#include "keyboard.h"
#include "string.h"
#include "type.h"

/** 定义显存区域，起始地址 0xB8000，大小0x8000(32KB) */
#define    VIDEO_MEMORY_BASE        0xB8000
#define    VIDEO_MEMORY_SIZE        0x8000

#define NR_TTY          12
#define NR_CONSOLE      (NR_TTY)


/**
 * 显示器信息，用于映射到显存指定部位
 */
typedef struct console {
    // 对应显存起始位置
    u32 video_original_addr;
    // 显存大小
    u32 video_mem_size;
    // 从 display_add 位置开始显示
    u32 display_addr;
    // 光标显示位置
    u32 cursor_pos;
} Console;

/**
 * 终端，每个终端对应一个交互式任务，这里定义的是终端任务数据结构，
 * 用循环队列存放键盘 key 的输入
 */
typedef struct tty {
    int head;
    int tail;
    int size;
    // 键盘 key 输入缓冲
    u32 buffer[50];
    Console *console_ptr;
} TTY;

/** tty 终端表 */
static TTY tty_table[NR_TTY];
/** 控制台列表 */
// TODO 感觉这里做一个 table 没什么必要性
static Console console_table[NR_CONSOLE];

/** 当前运行终端 */
static int current_tty;

/** tty 初始化 */
static void tty_init(TTY *tty_ptr);

/** 终端读 */
static void tty_read(int tty_no);

/** 终端写 */
static void tty_write(int tty_no);

/**
 * 写入console
 * TODO 本来在汇编里面显存到段起始地址是存在 gs 里面，但这里段 print_console 是没有办法控制汇编代码使用 gs
 * @param tty_no 当前 terminal 号
 * @param key 写入字符
 */
static void print_console(Console *console_ptr, char key);

/** 设置光标位置 */
static void set_cursor(unsigned int position);

/** 切换控制台 */
static void switch_console(Console *console);

/** 切换 tty */
static void switch_tty(int tty_no);

/** 将显示屏映射到另外一片内存 */
static void set_video_start_addr(u32 addr);

static void tty_init(TTY *tty_ptr) {
    // buffer
    tty_ptr->head = 1;
    tty_ptr->tail = 0;
    tty_ptr->size = 50;

    // tty 编号
    int nr_tty = tty_ptr - tty_table;
    // 设置 tty 对应的 console
    tty_ptr->console_ptr = console_table + nr_tty;

    // 每个 console 占用多少显存大小，因为一个字符本身有一个 byte，显示模式也要一个 byte，所以要除以 2
    int console_video_size = (VIDEO_MEMORY_SIZE >> 1) / NR_CONSOLE;

    // TODO 这里应该判断一下 tty 显存大小是否越界 （Limit: 0xB8000 + 0x8000）
    Console *console_ptr = tty_ptr->console_ptr;
    console_ptr->video_original_addr = nr_tty * console_video_size;
    console_ptr->video_mem_size = console_video_size;
    console_ptr->display_addr = console_ptr->video_original_addr;
    console_ptr->cursor_pos = console_ptr->video_original_addr;

    if (nr_tty == 0) {
        // 第一个 tty 任务的光标应该设置为 cursor_pos 处
        console_ptr->cursor_pos = cursor_pos / 2;
        cursor_pos = 0;
    } else {
        print_console(console_ptr, '0' + nr_tty);
        print_console(console_ptr, '#');
    }
}

static void tty_read(int tty_no) {
    if (current_tty == tty_no) {
        keyboard_read(tty_no);
    }
}

static void tty_write(int tty_no) {
    if (tty_no < 0 || tty_no >= NR_TTY) {
        return;
    }

    if (current_tty == tty_no) {
        TTY *tty_ptr = tty_table + tty_no;
        int next = (tty_ptr->tail + 1) % tty_ptr->size;
        if (next != tty_ptr->head) {
            u32 key = tty_ptr->buffer[next];
            tty_ptr->tail = next;
            // TODO key 是 32 位的，但 print_console 是 8 位的，会不会有问题
            print_console(tty_ptr->console_ptr, key);
        }
    }
}

/** tty 总进程，负责轮询所有的 tty */
void tty_task() {
    current_tty = 0;
    TTY *tty_ptr = 0;

    for (tty_ptr = tty_table; tty_ptr < tty_table + NR_TTY; tty_ptr++) {
        tty_init(tty_ptr);
    }

    while (True) {
        // TODO 为什么直接用 current_tty
//        for (int tty_no = 0; tty_no < NR_TTY; tty_no++) {
//            tty_read(tty_no);
//            tty_write(tty_no);
//        }

        tty_read(current_tty);
        tty_write(current_tty);

//        print_string("tty task query. ");
    }
}

void terminal_key_callback(u32 key, int tty_no) {
    if (!(key & FLAG_FUNCTION_KEY)) {
        // 可打印字符，写入 tty 缓冲
        int next = (tty_table[tty_no].head + 1) % tty_table[tty_no].size;
        if (next != tty_table[tty_no].tail) {
            tty_table[tty_no].buffer[tty_table[tty_no].head] = key;
            tty_table[tty_no].head = next;
        } else {
            // TODO 缓冲区溢出 do something
        }
    } else {
        // 处理功能 key
        int code = key & MASK_RAW;
        switch (code) {
            case F1:
            case F2:
            case F3:
            case F4:
            case F5:
            case F6:
            case F7:
            case F8:
            case F9:
            case F10:
            case F11:
            case F12:
                // ALT + F1 ~ F12
                if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {
                    switch_tty(code - F1);
                }
                break;
            default:
                break;
        }
    }
}


// TODO 代码整理下，太乱了
/* 用来显示屏的显示方式的常量 */
#define DEFAULT_CHAR_COLOR      0x07
#define CRTC_ADDR_REG           0x3D4    /* CRT Controller Registers - Addr Register */
#define CRTC_DATA_REG           0x3D5    /* CRT Controller Registers - Data Register */
#define START_ADDR_H            0xC    /* reg index of video mem start addr (MSB) */
#define START_ADDR_L            0xD    /* reg index of video mem start addr (LSB) */
#define CURSOR_H                0xE    /* reg index of cursor position (MSB) */
#define CURSOR_L                0xF    /* reg index of cursor position (LSB) */

static void print_console(Console *console_ptr, char key) {
    u8 *video_ptr = (u8 *) (VIDEO_MEMORY_BASE + console_ptr->cursor_pos * 2);
    *video_ptr++ = key;
    *video_ptr++ = DEFAULT_CHAR_COLOR;
    console_ptr->cursor_pos++;
    set_cursor(console_ptr->cursor_pos);
}

static void set_cursor(unsigned int position) {
    /**
     * TODO
     * @attention
     * 用户态程序不应该用 cli sti 指令，否则会报 #GP 错误
     * 问题是，tty 的确要去操纵中断硬件怎么办？
     * 从目前情况看来，系统每个时刻只能支持一个 tty，所以暂时还没有
     * 这个方面的担忧。
     *
     * 这个事情也说明了我脑海中还没有简历起一个内核"系统架构"的概念。
     */
//    cli();
    out_byte(CRTC_ADDR_REG, CURSOR_H);
    out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
    out_byte(CRTC_ADDR_REG, CURSOR_L);
    out_byte(CRTC_DATA_REG, position & 0xFF);
//    sti();
}

static void switch_tty(int tty_no) {
    if (current_tty == tty_no) {
        return;
    }

    if (tty_no < 0 || tty_no >= NR_TTY) {
        return;
    }

    // TODO 这里存在并发问题
    current_tty = tty_no;
    switch_console((tty_table + tty_no)->console_ptr);
}

static void switch_console(Console *console) {
    if (console == 0) {
        return;
    }
    set_video_start_addr(console->video_original_addr);
    set_cursor(console->cursor_pos);
}

static void set_video_start_addr(u32 addr) {
    // TODO 按理这儿应该会报错的，用户态不允许操作中断
    // 关闭中断
//    cli()
    out_byte(CRTC_ADDR_REG, START_ADDR_H);
    out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
    out_byte(CRTC_ADDR_REG, START_ADDR_L);
    out_byte(CRTC_DATA_REG, addr & 0xFF);
    // 开启中断
//    sti();
}