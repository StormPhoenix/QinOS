#ifndef _QUEUE_H_
#define _QUEUE_H_

#define DEFAULT_QUEUE_SIZE  128

// TODO 没有 class 类型不太好写
// 为什么不用 cpp 写呢？不如尝试下 cpp
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