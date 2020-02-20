/*
* type.h 定义一些宏类型
*/

#ifndef _TYPE_H_
#define _TYPE_H_

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;

// 中断处理函数指针
typedef void (*int_handler)();    

// static 修饰函数，让函数对外部文件不可见
#define PRIVATE     static

#endif