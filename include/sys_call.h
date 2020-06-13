#ifndef _SYS_CALL_H_
#define _SYS_CALL_H_

#include "type.h"

/** 系统调用出错号 **/
/** 调用号越界 */
#define NR_syscall_bounds           -57
/** 调用函数不存在 */
#define NR_syscall_undefine         -58

/** 系统调用中断号 */
#define INT_VECTOR_SYS_CALL     0x70

/** 系统调用号 */
#define __NR_get_ticks      0

/** 系统调用函数 */
// TODO 测试系统调用
extern int sys_get_ticks();

/** 系统调用表 */
system_call_ptr sys_call_table[] = {sys_get_ticks};

/** system call 表大小 */
int NR_syscalls = sizeof(sys_call_table) / sizeof(system_call_ptr);

/**
 * __syscall0__(type, name)
 * 无参数系统调用宏，用于生成无参数的系统调用函数
 * __NR_##name 是一个字符串拼接模板，用来动态生成系统调用号
 * */
#define __syscall0__(type, name) \
type name() { \
    long __res; \
    __asm__ volatile (\
        "int    $0x70" \
        : "=a" (__res) \
        : "0" (__NR_##name) \
            ); \
    if (__res >= 0) { \
        return (type) __res; \
    } \
    return (long) __res; \
}

// TODO 测试系统调用 get_ticks
__syscall0__(long, get_ticks)

/** 系统调用入口 */
extern int system_call();

#endif