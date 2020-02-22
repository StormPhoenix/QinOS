/*
* 内核起始文件
*/

#include "type.h"
#include "sys.h"
#include "const.h"


// gdt 数据结构
Descriptor gdt[GDT_SIZE];
// gdt 指针，这个变量在 kernel.asm 中会被赋值
u8 gdt_ptr[6];

// string.asm 
// TODO 这儿的 size 应该是无符号数字吗?
extern void copy_memory(void* src, int size, void* dst);
// bits.asm
extern void print_string(char *str);

void setup_gdt() 
{
    // 将 gdt_ptr 指向的 gdt 赋值给这个文件中的 gdt
    copy_memory( 
        (void*) (*((u32*) (&gdt_ptr[2]))),
        *((u16 *) (&gdt_ptr[0])) + 1,
        &gdt
    );

    *((u16* ) (&gdt_ptr[0])) = GDT_SIZE * sizeof(Descriptor) - 1;
    *((u32 *) (&gdt_ptr[2])) = (u32) &gdt;

    print_string("You superise mother-fucker!\n");
}