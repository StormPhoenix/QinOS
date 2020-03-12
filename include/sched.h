/*
 * sched.h
 * 
 * 进程相关数据结构定义
 */

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "task.h"
#include "mm/mm.h"
#include "pm.h"

// 同时存在的进程最大数量
#define NR_TASK   4

// 当前运行进程
Task *current_task;
// 进程表
Task *task_table[NR_TASK];


// OS 中第一个 Task
#define INIT_TASK { \
    /* tss */ \
    { \
        0, PAGE_SIZE + (u32) &init_task, 0x10, 0, 0, 0, 0, 0,\
        0, 0, 0, 0, 0, 0, 0, 0, \
        0, 0, 0x17, 0xf, 0x17, 0x17, 0x17, 0x1B, \
        _LDT(0), 0x0000, 0x8000, \
    }, \
    /* ldt */ \
    { \
        {0, 0}, \
        {0, 0}, \
        {0, 0}, \
    },\
    /* task_id */ \
    0,\
    /* start_time */ \
    0, \
    /* priority */ \
    50, \
    /* counter */ \
    50 \
}


/**
 * TODO
 * 程序初始化 Task，前期用来坐测试，后期要改过来
 */
void sched_init();


// TODO 测试代码要删掉
void delay();

void TestA();

void TestB();

void TestC();


/**
 * 进程调度程序
 */
void schedule();

#endif