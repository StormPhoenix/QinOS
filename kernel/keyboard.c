#include "bits.h"
#include "console.h"
#include "intr.h"
#include "keyboard.h"
#include "std/queue.h"
#include "string.h"
#include "thread.h"
#include "type.h"

/** 存放键盘码缓冲队列 */
Queue keyboard_buffer;

/** 键盘缓冲区的锁 */
static Mutex key_buff_mutex = {0};

/** 键盘映射表 */
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

/**
 * keyboard 初始化，内核态运行
 */
void keyboard_init();

/**
 * 键盘终端处理程序
 */
void do_keyboard();

/**
 * 读取 keyboard 缓冲
 */
static u8 get_key_from_buffer();

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

void keyboard_init() {
    // 初始化缓冲区
    keyboard_buffer.head = 1;
    keyboard_buffer.tail = 0;
    keyboard_buffer.size = DEFAULT_QUEUE_SIZE;
    enable_irq(IRQ_KEYBOARD);
}

void keyboard_read(int tty_no) {
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
            terminal_key_callback(key, tty_no);
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