/*
* const.h 
*
* 主要用于定义一些常量
*/

#ifndef _CONST_H_
#define _CONST_H_

#include "type.h"

// gdt 描述符的数量
#define GDT_SIZE    128
// idt 描述符的数量
#define IDT_SIZE    256

// 8295A 芯片的 in、out 端口
#define MASTER_8259_CTL 0x20
#define MASTER_8259_MASK 0x21
#define SLAVE_8259_CTL 0xa0
#define SLAVE_8259_MASK 0xa1

#endif