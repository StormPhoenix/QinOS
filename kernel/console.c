/**
 * 定义键盘处理操作，相关宏定义和 keymap
 * 处理流程太繁琐了，处理按键的操作都是从 Orange'S 复制过来的
 */

#include "asm/system.h"
#include "bits.h"
#include "console.h"
#include "intr.h"
#include "string.h"
#include "thread.h"
#include "type.h"

// TODO 移走这部分代码
/** 定义显存区域，起始地址 0xB8000，大小0x8000(32KB) */
#define    VIDEO_MEMORY_BASE        0xB8000
#define    VIDEO_MEMORY_SIZE        0x8000

#define NR_TTY          5
#define NR_CONSOLE      (NR_TTY)

// 扫描码数量
#define NR_SCAN_CODES           0x80
#define MAP_COLS                3
#define FLAG_BREAK              0x0080        /* Break Code			*/
#define FLAG_FUNCTION_KEY       0x0100        /* Normal function keys		*/
#define FLAG_SHIFT_L            0x0200        /* Shift key			*/
#define FLAG_SHIFT_R            0x0400        /* Shift key			*/
#define FLAG_CTRL_L             0x0800        /* Control key			*/
#define FLAG_CTRL_R             0x1000        /* Control key			*/
#define FLAG_ALT_L              0x2000        /* Alternate key		*/
#define FLAG_ALT_R              0x4000        /* Alternate key		*/
#define FLAG_PAD                0x8000        /* keys in num pad		*		*/


/* Special keys */
#define ESC                     (0x01 + FLAG_FUNCTION_KEY)    /* Esc		*/
#define TAB                     (0x02 + FLAG_FUNCTION_KEY)    /* Tab		*/
#define ENTER                   (0x03 + FLAG_FUNCTION_KEY)    /* Enter	*/
#define BACKSPACE               (0x04 + FLAG_FUNCTION_KEY)    /* BackSpace	*/

#define GUI_L                   (0x05 + FLAG_FUNCTION_KEY)    /* L GUI	*/
#define GUI_R                   (0x06 + FLAG_FUNCTION_KEY)    /* R GUI	*/
#define APPS                    (0x07 + FLAG_FUNCTION_KEY)    /* APPS	*/

/* Shift, Ctrl, Alt */
#define SHIFT_L                 (0x08 + FLAG_FUNCTION_KEY)    /* L Shift	*/
#define SHIFT_R                 (0x09 + FLAG_FUNCTION_KEY)    /* R Shift	*/
#define CTRL_L                  (0x0A + FLAG_FUNCTION_KEY)    /* L Ctrl	*/
#define CTRL_R                  (0x0B + FLAG_FUNCTION_KEY)    /* R Ctrl	*/
#define ALT_L                   (0x0C + FLAG_FUNCTION_KEY)    /* L Alt	*/
#define ALT_R                   (0x0D + FLAG_FUNCTION_KEY)    /* R Alt	*/

/* Lock keys */
#define CAPS_LOCK               (0x0E + FLAG_FUNCTION_KEY)    /* Caps Lock	*/
#define NUM_LOCK                (0x0F + FLAG_FUNCTION_KEY)    /* Number Lock	*/
#define SCROLL_LOCK             (0x10 + FLAG_FUNCTION_KEY)    /* Scroll Lock	*/

/* Function keys */
#define F1                      (0x11 + FLAG_FUNCTION_KEY)    /* F1		*/
#define F2                      (0x12 + FLAG_FUNCTION_KEY)    /* F2		*/
#define F3                      (0x13 + FLAG_FUNCTION_KEY)    /* F3		*/
#define F4                      (0x14 + FLAG_FUNCTION_KEY)    /* F4		*/
#define F5                      (0x15 + FLAG_FUNCTION_KEY)    /* F5		*/
#define F6                      (0x16 + FLAG_FUNCTION_KEY)    /* F6		*/
#define F7                      (0x17 + FLAG_FUNCTION_KEY)    /* F7		*/
#define F8                      (0x18 + FLAG_FUNCTION_KEY)    /* F8		*/
#define F9                      (0x19 + FLAG_FUNCTION_KEY)    /* F9		*/
#define F10                     (0x1A + FLAG_FUNCTION_KEY)    /* F10		*/
#define F11                     (0x1B + FLAG_FUNCTION_KEY)    /* F11		*/
#define F12                     (0x1C + FLAG_FUNCTION_KEY)    /* F12		*/

/* Control Pad */
#define PRINTSCREEN             (0x1D + FLAG_FUNCTION_KEY)    /* Print Screen	*/
#define PAUSEBREAK              (0x1E + FLAG_FUNCTION_KEY)    /* Pause/Break	*/
#define INSERT                  (0x1F + FLAG_FUNCTION_KEY)    /* Insert	*/
#define DELETE                  (0x20 + FLAG_FUNCTION_KEY)    /* Delete	*/
#define HOME                    (0x21 + FLAG_FUNCTION_KEY)    /* Home		*/
#define END                     (0x22 + FLAG_FUNCTION_KEY)    /* End		*/
#define PAGEUP                  (0x23 + FLAG_FUNCTION_KEY)    /* Page Up	*/
#define PAGEDOWN                (0x24 + FLAG_FUNCTION_KEY)    /* Page Down	*/
#define UP                      (0x25 + FLAG_FUNCTION_KEY)    /* Up		*/
#define DOWN                    (0x26 + FLAG_FUNCTION_KEY)    /* Down		*/
#define LEFT                    (0x27 + FLAG_FUNCTION_KEY)    /* Left		*/
#define RIGHT                   (0x28 + FLAG_FUNCTION_KEY)    /* Right	*/

/* ACPI keys */
#define POWER                   (0x29 + FLAG_FUNCTION_KEY)    /* Power	*/
#define SLEEP                   (0x2A + FLAG_FUNCTION_KEY)    /* Sleep	*/
#define WAKE                    (0x2B + FLAG_FUNCTION_KEY)    /* Wake Up	*/

/* Num Pad */
#define PAD_SLASH               (0x2C + FLAG_FUNCTION_KEY)    /* /		*/
#define PAD_STAR                (0x2D + FLAG_FUNCTION_KEY)    /* *		*/
#define PAD_MINUS               (0x2E + FLAG_FUNCTION_KEY)    /* -		*/
#define PAD_PLUS                (0x2F + FLAG_FUNCTION_KEY)    /* +		*/
#define PAD_ENTER               (0x30 + FLAG_FUNCTION_KEY)    /* Enter	*/
#define PAD_DOT                 (0x31 + FLAG_FUNCTION_KEY)    /* .		*/
#define PAD_0                   (0x32 + FLAG_FUNCTION_KEY)    /* 0		*/
#define PAD_1                   (0x33 + FLAG_FUNCTION_KEY)    /* 1		*/
#define PAD_2                   (0x34 + FLAG_FUNCTION_KEY)    /* 2		*/
#define PAD_3                   (0x35 + FLAG_FUNCTION_KEY)    /* 3		*/
#define PAD_4                   (0x36 + FLAG_FUNCTION_KEY)    /* 4		*/
#define PAD_5                   (0x37 + FLAG_FUNCTION_KEY)    /* 5		*/
#define PAD_6                   (0x38 + FLAG_FUNCTION_KEY)    /* 6		*/
#define PAD_7                   (0x39 + FLAG_FUNCTION_KEY)    /* 7		*/
#define PAD_8                   (0x3A + FLAG_FUNCTION_KEY)    /* 8		*/
#define PAD_9                   (0x3B + FLAG_FUNCTION_KEY)    /* 9		*/
#define PAD_UP                  PAD_8            /* Up		*/
#define PAD_DOWN                PAD_2            /* Down		*/
#define PAD_LEFT                PAD_4            /* Left		*/
#define PAD_RIGHT               PAD_6            /* Right	*/
#define PAD_HOME                PAD_7            /* Home		*/
#define PAD_END                 PAD_1            /* End		*/
#define PAD_PAGEUP              PAD_9            /* Page Up	*/
#define PAD_PAGEDOWN            PAD_3            /* Page Down	*/
#define PAD_INS                 PAD_0            /* Ins		*/
#define PAD_MID                 PAD_5            /* Middle key	*/
#define PAD_DEL                 PAD_DOT            /* Del		*/

/** ---------------- 控制键状态 -------------------- */
static int code_with_E0 = 0;
/* l shift state */
static int status_left_shift;
/* r shift state */
static int status_right_shift;
/* l alt state	 */
static int status_alt_left;
/* r left state	 */
static int status_right_alt;
/* l ctrl state	 */
static int status_ctrl_left;
/* l ctrl state	 */
static int status_ctrl_right;
/* Caps Lock	 */
static int status_caps_lock;
/* Num Lock	 */
static int status_num_lock;
/* Scroll Lock	 */
static int status_scroll_lock;
/* Row in keymap array */
static int column;

/** ----------------- 键盘映射表 -------------------- */
u32 keymap[NR_SCAN_CODES * MAP_COLS] = {
/* scan-code			!Shift		Shift		E0 XX	*/
/* ==================================================================== */
/* 0x00 - none		*/    0, 0, 0,
/* 0x01 - ESC		*/    ESC, ESC, 0,
/* 0x02 - '1'		*/    '1', '!', 0,
/* 0x03 - '2'		*/    '2', '@', 0,
/* 0x04 - '3'		*/    '3', '#', 0,
/* 0x05 - '4'		*/    '4', '$', 0,
/* 0x06 - '5'		*/    '5', '%', 0,
/* 0x07 - '6'		*/    '6', '^', 0,
/* 0x08 - '7'		*/    '7', '&', 0,
/* 0x09 - '8'		*/    '8', '*', 0,
/* 0x0A - '9'		*/    '9', '(', 0,
/* 0x0B - '0'		*/    '0', ')', 0,
/* 0x0C - '-'		*/    '-', '_', 0,
/* 0x0D - '='		*/    '=', '+', 0,
/* 0x0E - BS		*/    BACKSPACE, BACKSPACE, 0,
/* 0x0F - TAB		*/    TAB, TAB, 0,
/* 0x10 - 'q'		*/    'q', 'Q', 0,
/* 0x11 - 'w'		*/    'w', 'W', 0,
/* 0x12 - 'e'		*/    'e', 'E', 0,
/* 0x13 - 'r'		*/    'r', 'R', 0,
/* 0x14 - 't'		*/    't', 'T', 0,
/* 0x15 - 'y'		*/    'y', 'Y', 0,
/* 0x16 - 'u'		*/    'u', 'U', 0,
/* 0x17 - 'i'		*/    'i', 'I', 0,
/* 0x18 - 'o'		*/    'o', 'O', 0,
/* 0x19 - 'p'		*/    'p', 'P', 0,
/* 0x1A - '['		*/    '[', '{', 0,
/* 0x1B - ']'		*/    ']', '}', 0,
/* 0x1C - CR/LF		*/    ENTER, ENTER, PAD_ENTER,
/* 0x1D - l. Ctrl	*/    CTRL_L, CTRL_L, CTRL_R,
/* 0x1E - 'a'		*/    'a', 'A', 0,
/* 0x1F - 's'		*/    's', 'S', 0,
/* 0x20 - 'd'		*/    'd', 'D', 0,
/* 0x21 - 'f'		*/    'f', 'F', 0,
/* 0x22 - 'g'		*/    'g', 'G', 0,
/* 0x23 - 'h'		*/    'h', 'H', 0,
/* 0x24 - 'j'		*/    'j', 'J', 0,
/* 0x25 - 'k'		*/    'k', 'K', 0,
/* 0x26 - 'l'		*/    'l', 'L', 0,
/* 0x27 - ';'		*/    ';', ':', 0,
/* 0x28 - '\''		*/    '\'', '"', 0,
/* 0x29 - '`'		*/    '`', '~', 0,
/* 0x2A - l. SHIFT	*/    SHIFT_L, SHIFT_L, 0,
/* 0x2B - '\'		*/    '\\', '|', 0,
/* 0x2C - 'z'		*/    'z', 'Z', 0,
/* 0x2D - 'x'		*/    'x', 'X', 0,
/* 0x2E - 'c'		*/    'c', 'C', 0,
/* 0x2F - 'v'		*/    'v', 'V', 0,
/* 0x30 - 'b'		*/    'b', 'B', 0,
/* 0x31 - 'n'		*/    'n', 'N', 0,
/* 0x32 - 'm'		*/    'm', 'M', 0,
/* 0x33 - ','		*/    ',', '<', 0,
/* 0x34 - '.'		*/    '.', '>', 0,
/* 0x35 - '/'		*/    '/', '?', PAD_SLASH,
/* 0x36 - r. SHIFT	*/    SHIFT_R, SHIFT_R, 0,
/* 0x37 - '*'		*/    '*', '*', 0,
/* 0x38 - ALT		*/    ALT_L, ALT_L, ALT_R,
/* 0x39 - ' '		*/    ' ', ' ', 0,
/* 0x3A - CapsLock	*/    CAPS_LOCK, CAPS_LOCK, 0,
/* 0x3B - F1		*/    F1, F1, 0,
/* 0x3C - F2		*/    F2, F2, 0,
/* 0x3D - F3		*/    F3, F3, 0,
/* 0x3E - F4		*/    F4, F4, 0,
/* 0x3F - F5		*/    F5, F5, 0,
/* 0x40 - F6		*/    F6, F6, 0,
/* 0x41 - F7		*/    F7, F7, 0,
/* 0x42 - F8		*/    F8, F8, 0,
/* 0x43 - F9		*/    F9, F9, 0,
/* 0x44 - F10		*/    F10, F10, 0,
/* 0x45 - NumLock	*/    NUM_LOCK, NUM_LOCK, 0,
/* 0x46 - ScrLock	*/    SCROLL_LOCK, SCROLL_LOCK, 0,
/* 0x47 - Home		*/    PAD_HOME, '7', HOME,
/* 0x48 - CurUp		*/    PAD_UP, '8', UP,
/* 0x49 - PgUp		*/    PAD_PAGEUP, '9', PAGEUP,
/* 0x4A - '-'		*/    PAD_MINUS, '-', 0,
/* 0x4B - Left		*/    PAD_LEFT, '4', LEFT,
/* 0x4C - MID		*/    PAD_MID, '5', 0,
/* 0x4D - Right		*/    PAD_RIGHT, '6', RIGHT,
/* 0x4E - '+'		*/    PAD_PLUS, '+', 0,
/* 0x4F - End		*/    PAD_END, '1', END,
/* 0x50 - Down		*/    PAD_DOWN, '2', DOWN,
/* 0x51 - PgDown	*/    PAD_PAGEDOWN, '3', PAGEDOWN,
/* 0x52 - Insert	*/    PAD_INS, '0', INSERT,
/* 0x53 - Delete	*/    PAD_DOT, '.', DELETE,
/* 0x54 - Enter		*/    0, 0, 0,
/* 0x55 - ???		*/    0, 0, 0,
/* 0x56 - ???		*/    0, 0, 0,
/* 0x57 - F11		*/    F11, F11, 0,
/* 0x58 - F12		*/    F12, F12, 0,
/* 0x59 - ???		*/    0, 0, 0,
/* 0x5A - ???		*/    0, 0, 0,
/* 0x5B - ???		*/    0, 0, GUI_L,
/* 0x5C - ???		*/    0, 0, GUI_R,
/* 0x5D - ???		*/    0, 0, APPS,
/* 0x5E - ???		*/    0, 0, 0,
/* 0x5F - ???		*/    0, 0, 0,
/* 0x60 - ???		*/    0, 0, 0,
/* 0x61 - ???		*/    0, 0, 0,
/* 0x62 - ???		*/    0, 0, 0,
/* 0x63 - ???		*/    0, 0, 0,
/* 0x64 - ???		*/    0, 0, 0,
/* 0x65 - ???		*/    0, 0, 0,
/* 0x66 - ???		*/    0, 0, 0,
/* 0x67 - ???		*/    0, 0, 0,
/* 0x68 - ???		*/    0, 0, 0,
/* 0x69 - ???		*/    0, 0, 0,
/* 0x6A - ???		*/    0, 0, 0,
/* 0x6B - ???		*/    0, 0, 0,
/* 0x6C - ???		*/    0, 0, 0,
/* 0x6D - ???		*/    0, 0, 0,
/* 0x6E - ???		*/    0, 0, 0,
/* 0x6F - ???		*/    0, 0, 0,
/* 0x70 - ???		*/    0, 0, 0,
/* 0x71 - ???		*/    0, 0, 0,
/* 0x72 - ???		*/    0, 0, 0,
/* 0x73 - ???		*/    0, 0, 0,
/* 0x74 - ???		*/    0, 0, 0,
/* 0x75 - ???		*/    0, 0, 0,
/* 0x76 - ???		*/    0, 0, 0,
/* 0x77 - ???		*/    0, 0, 0,
/* 0x78 - ???		*/    0, 0, 0,
/* 0x78 - ???		*/    0, 0, 0,
/* 0x7A - ???		*/    0, 0, 0,
/* 0x7B - ???		*/    0, 0, 0,
/* 0x7C - ???		*/    0, 0, 0,
/* 0x7D - ???		*/    0, 0, 0,
/* 0x7E - ???		*/    0, 0, 0,
/* 0x7F - ???		*/    0, 0, 0
};

/**
 * 显示器信息，用于映射到显存指定部位
 */
typedef struct console {
    // 对应显存起始位置
    u32 start_video_mem_addr;
    // 显存大小
    u32 video_mem_size;
    // 当前显示地址
    u32 current_video_mem_addr;
    // 光标显示位置
    u32 cursor;
} Console;

/**
 * 终端，每个终端对应一个交互式任务
 */
typedef struct tty {
    int head;
    int tail;
    int size;
    u32 buffer[50];
    Console *console;
} TTY;

/** tty 终端表 */
static TTY tty_table[NR_TTY];
static Console console_table[NR_CONSOLE];

/** 当前运行终端 */
static int current_tty;

// TODO 更换地方
/** 键盘缓冲区的锁 */
static Mutex key_buff_mutex = {0};

/** 读取 keyboard 缓冲 */
static u8 get_key_from_buffer();

// TODO 迁移到 keyboard.c
/** tty task 执行 */
static void keyboard_read(int tty_no);

/** tty 初始化 */
static void tty_init(int tty_no);

/** 终端读 */
static void tty_read(int tty_no);

/** 终端写 */
static void tty_write(int tty_no);

/** 写入显示器 */
static void print_console(int tty_no, char key);

/** 设置光标位置 */
static void set_cursor(unsigned int position);

/** 键盘中断 */
void do_keyboard() {
    u8 code = in_byte(0x60);
    // TODO delete ?
//    request_lock(&key_buff_mutex);
    // 入队
    int next = (keyboard_buffer.head + 1) % keyboard_buffer.size;
    if (next != keyboard_buffer.tail) {
        keyboard_buffer.buffer[keyboard_buffer.head] = code;
        keyboard_buffer.head = next;
    } else {
        print_string("keyboard buffer is full.\n");
    }
//    release_lock(&key_buff_mutex);
}

/** 初始化 keyboard */
void keyboard_init() {
    // 初始化缓冲区
    keyboard_buffer.head = 1;
    keyboard_buffer.tail = 0;
    keyboard_buffer.size = 128;
    enable_irq(IRQ_KEYBOARD);
}

static void tty_init(int tty_no) {
    // buffer
    tty_table[tty_no].head = 1;
    tty_table[tty_no].tail = 0;
    tty_table[tty_no].size = 50;
    // console
    Console *console = &console_table[tty_no];
    // 每个 console 占用多少显存大小
    int console_video_size = VIDEO_MEMORY_SIZE / NR_CONSOLE;
    console->start_video_mem_addr = VIDEO_MEMORY_BASE + tty_no * console_video_size;
    console->video_mem_size = console_video_size;
    console->current_video_mem_addr = console->start_video_mem_addr;
    console->cursor = 0;
    set_cursor(console->cursor);

    tty_table[tty_no].console = console;
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

static void in_process(u32 key, int tty_no) {
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

static u8 get_key_from_buffer() {
    u8 scan_code;

    while (True) {
        request_lock(&key_buff_mutex);
        int next = (keyboard_buffer.tail + 1) % keyboard_buffer.size;
        if (next % keyboard_buffer.size != keyboard_buffer.head) {
            // key buffer not empty
            scan_code = keyboard_buffer.buffer[next];
            keyboard_buffer.tail = next;
            break;
        }
        release_lock(&key_buff_mutex);
    }
    release_lock(&key_buff_mutex);
    return scan_code;
}

static void keyboard_read(int tty_no) {
    // 扫描码
    u8 scan_code;
    char output[2];
    // make 表示按键是按下还是弹起，1：按下，0：弹起
    int make;
    // 8 位扫描码转化成 32 位key
    u32 key = 0;
    // 索引 keymap[] 的某一行
    u32 *keyrow;
    // 有的扫描码是 E0 开头，表示是一些特殊键，要区分对待
    code_with_E0 = 0;
    scan_code = get_key_from_buffer();

    /* 下面开始解析扫描码 */
    if (scan_code == 0xE1) {
        int i;
        u8 pausebrk_scode[] = {0xE1, 0x1D, 0x45,
                               0xE1, 0x9D, 0xC5};
        int is_pausebreak = 1;
        for (i = 1; i < 6; i++) {
            if (get_key_from_buffer() != pausebrk_scode[i]) {
                is_pausebreak = 0;
                break;
            }
        }
        if (is_pausebreak) {
            key = PAUSEBREAK;
        }
    } else if (scan_code == 0xE0) {
        // scan_code 是控制键
        scan_code = get_key_from_buffer();

        // PrintScreen 被按下
        if (scan_code == 0x2A) {
            if (get_key_from_buffer() == 0xE0) {
                if (get_key_from_buffer() == 0x37) {
                    key = PRINTSCREEN;
                    make = 1;
                }
            }
        }
        /* PrintScreen 被释放 */
        if (scan_code == 0xB7) {
            if (get_key_from_buffer() == 0xE0) {
                if (get_key_from_buffer() == 0xAA) {
                    key = PRINTSCREEN;
                    make = 0;
                }
            }
        }
        /* 不是PrintScreen, 此时scan_code为0xE0紧跟的那个值. */
        if (key == 0) {
            code_with_E0 = 1;
        }
    }

    if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
        // 首先判断Make Code 还是 Break Code
        make = (scan_code & FLAG_BREAK ? 0 : 1);
        keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];
        column = 0;
        // 检测是否按下了 shift 键
        if (status_left_shift || status_right_shift) {
            column = 1;
        }

        if (code_with_E0) {
            column = 2;
            code_with_E0 = 0;
        }

        key = keyrow[column];
        switch (key) {
            case SHIFT_L:
                status_left_shift = make;
                break;
            case SHIFT_R:
                status_right_shift = make;
                break;
            case CTRL_L:
                status_ctrl_left = make;
                break;
            case CTRL_R:
                status_ctrl_right = make;
                break;
            case ALT_L:
                status_alt_left = make;
                break;
            case ALT_R:
                status_alt_left = make;
                break;
            default:
                break;
        }

        if (make) {
            // 只处理按下的键
            key |= status_left_shift ? FLAG_SHIFT_L : 0;
            key |= status_right_shift ? FLAG_SHIFT_R : 0;
            key |= status_ctrl_left ? FLAG_CTRL_L : 0;
            key |= status_ctrl_right ? FLAG_CTRL_R : 0;
            key |= status_alt_left ? FLAG_ALT_L : 0;
            key |= status_right_alt ? FLAG_ALT_R : 0;

            in_process(key, tty_no);
        }
    }
}

// TODO 代码整理下，太乱了
#define DEFAULT_CHAR_COLOR      0x07
#define CRTC_ADDR_REG           0x3D4    /* CRT Controller Registers - Addr Register */
#define CRTC_DATA_REG           0x3D5    /* CRT Controller Registers - Data Register */
#define START_ADDR_H            0xC    /* reg index of video mem start addr (MSB) */
#define START_ADDR_L            0xD    /* reg index of video mem start addr (LSB) */
#define CURSOR_H                0xE    /* reg index of cursor position (MSB) */
#define CURSOR_L                0xF    /* reg index of cursor position (LSB) */

static void print_console(int tty_no, char key) {
    Console *console = tty_table[tty_no].console;
    u8 *video_ptr = (u8 *) console->current_video_mem_addr;
    *video_ptr++ = key;
    *video_ptr++ = DEFAULT_CHAR_COLOR;
    console->current_video_mem_addr += 2;
    console->cursor++;
    set_cursor(console->cursor);
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