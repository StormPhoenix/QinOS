#include "string.h"
#include "type.h"

/**
 * 数字转字符串
 * @param str
 * @param num
 * @return
 */
char *itoa(char *str, int num) {
    char *p = str;
    char ch;
    int i;
    int flag = False;

    *p++ = '0';
    *p++ = 'x';

    if (num == 0) {
        *p++ = '0';
    } else {
        for (i = 28; i >= 0; i -= 4) {
            ch = (num >> i) & 0xF;
            if (flag || (ch > 0)) {
                flag = True;
                ch += '0';
                if (ch > '9') {
                    ch += 7;
                }
                *p++ = ch;
            }
        }
    }
    *p = 0;
    return str;
}

void print_hex(int input) {
    char output[16];
    itoa(output, input);
    print_string(output);
}

