/**
 * protect.h
 *
 * 包含系统保护模式模块的调用接口
 */

#ifndef _PROTECT_H_
#define _PROTECT_H_


#include "type.h"
#include "mm/mm.h"

/** 特权级数值 */
#define DPL_USER            3
#define DPL_KERNEL          0

// 内核代码段选择符
#define SELECTOR_KERNEL_CODE    0x8
// 内核数据段选择符
#define SELECTOR_KERNEL_DATA    0x10
// 内核显存区域选择符号
#define SELECTOR_VIDEO          0x18


/** 请求特权级数值 */
#define RPL_USER            3
#define RPL_KERNEL          0

/** 选择符 MASK */
#define SELECTOR_RPL_MASK   0xfffc
#define SELECTOR_TI_MASK    0xfffb

/** 选择符 GDT、LDT */
#define SELECTOR_TI_LDT     4
#define SELECTOR_TI_GDT     0

/** 代码段描述符属性 */
#define CODE_ATTR386        (u8)0x98
#define CODE_ATTR386_A      (u8)0x99
#define CODE_ATTR386_R      (u8)0x9A
#define CODE_ATTR386_RA     (u8)0x9B
#define CODE_ATTR386_C      (u8)0x9C
#define CODE_ATTR386_CA     (u8)0x9D
#define CODE_ATTR386_CR     (u8)0x9E
#define CODE_ATTR386_CRA    (u8)0x9F

/** 数据段描述符属性 */
#define DATA_ATTR386_R      (u8)0x90
#define DATA_ATTR386_RA     (u8)0x91
#define DATA_ATTR386_RW     (u8)0x92
#define DATA_ATTR386_RWA    (u8)0x93
#define DATA_ATTR386_RE     (u8)0x94
#define DATA_ATTR386_REA    (u8)0x95
#define DATA_ATTR386_REW    (u8)0x96
#define DATA_ATTR386_REWA   (u8)0x97

/** 系统段描述符属性 */
#define SYS_ATTR386_TSS     (u8)0x89        /** 32 位TSS段 */
#define SYS_ATTR386_LDT     (u8)0x82


/**
 * gdt、ldt 描述符表项
 */
typedef struct descriptor {
    // 段限长，0 - 15 位
    u16 limit_low;
    // 基地址，16 - 31 位
    u16 base_low;
    // 基地址，32 - 39 位
    u8 base_mid;
    // 属性1，40 - 47
    u8 attributes;
    // 4 位段限长 + 4 位属性，48 - 55 位
    u8 limit_high_and_attributes;
    // 基地址，56 - 63 位
    u8 base_high;
} Descriptor;


/**
 * 门描述符
 */
typedef struct gate {
    // 段中偏移值低16位
    u16 offset_low;
    // 段选择符
    u16 selector;
    // 参数个数
    u8 param_count;
    // type、dpl、p
    u8 attributs;
    // 段中偏移值高16位
    u16 offset_high;
} Gate;

#define GDT_SIZE        128
#define IDT_SIZE        256
#define MAX_LDT_SIZE    3
#define NR_IRQ          16

/** 第一个任务的 TSS、LDT */
#define FIRST_TSS_ENTRY     4
#define FIRST_LDT_ENTRY     FIRST_TSS_ENTRY + 1

/** 计算第n个任务的 TSS、LDT 段选择符的偏移值 */
#define _TSS(n)     ((((u32) n) << 4) + (FIRST_TSS_ENTRY << 3))
#define _LDT(n)     ((((u32) n) << 4) + (FIRST_LDT_ENTRY << 3))

// gdt 表
Descriptor gdt[GDT_SIZE];
// idt 表
Gate idt[IDT_SIZE];
// 页目录
u32 *page_dir;
// gdt 指针，i386用于加载GDT
u8 gdt_ptr[6];
// idt 指针，i386用于加载IDT
u8 idt_ptr[6];


/**
 * 取得 GDT 表中 selector 索引段指向的线性地址
 * @param selector GDT 索引
 * @return 32位线性地址
 */
u32 segment_to_linear_addr(u16 selector);


/**
 * 虚拟地址转化成线性地址
 * @param selector  GDT 段索引
 * @param offset 段内偏移地址
 * @return
 */
u32 virtual_to_linear_addr(u16 selector, u32 offset);


#endif