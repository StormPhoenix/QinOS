#ifndef _THREAD_H_
#define _THREAD_H_

#include "type.h"

typedef struct struct_mutex {
    volatile char atom;
} Mutex;

/** TODO 上锁加锁代码实现的太粗糙了，release_lock() 如果在
 * request_lock() 之前被调用会有bug */

/** 上锁 */
void request_lock(Mutex *mutex) {
    __asm__( \
        "loop:\n\t" \
        "xchgb %0, %1\n\t" \
        "cmpb $0x01, %0\n\t" \
        "je loop\n\t" \
        : \
        : "al" (0x01), "m" (*((char *) &(mutex->atom))) \
    );
}


/** 释放锁 */
void release_lock(Mutex *mutex) {
    __asm__( \
        "movb %%al, %1" \
        : \
        : "al" (0x00), "m" (*((char *) &(mutex->atom))) \
        );
}


#endif