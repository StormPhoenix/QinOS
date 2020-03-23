#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

// 扫描码数量
#define NR_SCAN_CODES           0x80
// keymap 映射行数
#define MAP_COLS                3
// 控制键标记
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

/**
 * 从外设键盘读取字符
 * @param tty_no 当前终端号
 */
void keyboard_read(int tty_no);


#endif