/**
 * 定义键盘处理操作，相关宏定义和 keymap
 * 处理流程太繁琐了，处理按键的操作都是从 Orange'S 复制过来的
 */

#include "asm/system.h"
#include "bits.h"
#include "console.h"
#include "intr.h"
#include "keyboard.h"
#include "string.h"
#include "type.h"

/** 定义显存区域，起始地址 0xB8000，大小0x8000(32KB) */
#define    VIDEO_MEMORY_BASE        0xB8000
#define    VIDEO_MEMORY_SIZE        0x8000

#define NR_TTY          5
#define NR_CONSOLE      (NR_TTY)


/**
 * 显示器信息，用于映射到显存指定部位
 */
typedef struct console {
    // 对应显存起始位置
    u32 video_memory_start_address;
    // 显存大小
    u32 video_memory_size;
    // 当前显示地址
    u32 current_display_address;
    // 光标显示位置
    u32 cursor_position;
} Console;

/**
 * 终端，每个终端对应一个交互式任务
 */
typedef struct tty {
    int head;
    int tail;
    int size;
    u32 buffer[50];
    Console *console_ptr;
} TTY;

/** tty 终端表 */
static TTY tty_table[NR_TTY];
static Console console_table[NR_CONSOLE];

/** 当前运行终端 */
static int current_tty;

/** tty 初始化 */
static void tty_init(int tty_no);

/** 终端读 */
static void tty_read(int tty_no);

/** 终端写 */
static void tty_write(int tty_no);

/**
 * 写入console
 * @param tty_no 当前 terminal 号
 * @param key 写入字符
 */
static void print_console(int tty_no, char key);

/** 设置光标位置 */
static void set_cursor(unsigned int position);

static void tty_init(int tty_no) {
    // buffer
    tty_table[tty_no].head = 1;
    tty_table[tty_no].tail = 0;
    tty_table[tty_no].size = 50;
    // console
    Console *console_ptr = &console_table[tty_no];
    // 每个 console 占用多少显存大小
    int console_video_size = VIDEO_MEMORY_SIZE / NR_CONSOLE;
    console_ptr->video_memory_start_address = VIDEO_MEMORY_BASE + tty_no * console_video_size;
    console_ptr->video_memory_size = console_video_size;
    console_ptr->current_display_address = console_ptr->video_memory_start_address;
    console_ptr->cursor_position = 0;
    set_cursor(console_ptr->cursor_position);

    tty_table[tty_no].console_ptr = console_ptr;
}

static void tty_read(int tty_no) {
    if (current_tty == tty_no) {
        keyboard_read(tty_no);
    }
}

static void tty_write(int tty_no) {
    if (current_tty == tty_no) {
        TTY *tty_ptr = &tty_table[tty_no];
        int next = (tty_ptr->tail + 1) % tty_ptr->size;
        if (next != tty_ptr->head) {
            u32 key = tty_ptr->buffer[next];
            tty_ptr->tail = next;
            // TODO key 是 32 位的，但 print_console 是 8 位的，会不会有问题
            print_console(tty_no, key);
        }
    }
}

/** tty 总进程，负责轮询所有的 tty */
void tty_task() {
    current_tty = 0;

    for (int tty_no = 0; tty_no < NR_TTY; tty_no++) {
        tty_init(tty_no);
    }

    while (True) {
        // TODO 为什么直接用 current_tty
        for (int tty_no = 0; tty_no < NR_TTY; tty_no++) {
            tty_read(tty_no);
            tty_write(tty_no);
        }
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

static void print_console(int tty_no, char key) {
    Console *console_ptr = tty_table[tty_no].console_ptr;
    u8 *video_ptr = (u8 *) console_ptr->current_display_address;
    *video_ptr++ = key;
    *video_ptr++ = DEFAULT_CHAR_COLOR;
    console_ptr->current_display_address += 2;
    console_ptr->cursor_position++;
    set_cursor(console_ptr->cursor_position);
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