#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "type.h"

#define DEFAULT_QUEUE_SIZE  128


// TODO 没有 class 类型不太好写
// 忘记了，C 没有泛型，没办法写出
// 为什么不用 cpp 写呢？不如尝试下 cpp
// 等内核支持 new 了，再在这个地方改写 Queue
typedef struct queue {
    // 队列中下一条空位的 index
    int head;
    // 队尾 index
    int tail;
    // 缓冲区大小
    int size;
    // 键盘缓冲
    char buffer[DEFAULT_QUEUE_SIZE];
} Queue;

#endif