#ifndef _SYS_H_
#define _SYS_H_

#include "type.h"

// 系统段描述符
typedef struct descriptor
{
    // 段限长，0 - 15 位
    u16 limitLow;
    // 基地址，16 - 31 位
    u16 baseLow;
    // 基地址，32 - 39 位
    u8 baseMid;
    // 属性1，40 - 47
    u8 attributes;
    // 4 位段限长 + 4 位属性，48 - 55 位
    u8 limitHighAndAttributes;
    // 基地址，56 - 63 位
    u8 baseHigh;
} Descriptor;

#endif