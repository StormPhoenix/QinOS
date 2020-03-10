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
typedef void (*task_function)();
typedef int (*system_call_ptr)();

// static 修饰函数，让函数对外部文件不可见
#define PRIVATE     static

// bool 类型
#define True    1
#define False   0

#endif