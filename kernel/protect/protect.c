#include "protect.h"
#include "string.h"
#include "sys.h"


void setup_paging() {
    // 页面表被放置在内存 0 起始处
    page_dir = 0;
}


void setup_gdt() {
    sgdt(gdt_ptr);
    memory_set(gdt, 0, sizeof(Descriptor) * GDT_SIZE);
    // 将 gdt_ptr 指向的 gdt 赋值给这个文件中的 gdt
    memory_copy(
            (void *) (*((u32 *) (&gdt_ptr[2]))),
            (int) (*((u16 *) (&gdt_ptr[0])) + 1),
            (void *) &gdt
    );

    *((u16 *) (&gdt_ptr[0])) = GDT_SIZE * sizeof(Descriptor) - 1;
    *((u32 *) (&gdt_ptr[2])) = (u32) &gdt;
    lgdt(gdt_ptr);
    print_string("You superise mother-fucker!\n");
}


void set_gdt_descriptor(Descriptor *descriptor, u32 base, u32 limit, u16 attributes) {
    // 参考 i386 手册设置 descriptor
    descriptor->limit_low = limit & 0xffff;
    descriptor->base_low = base & 0xffff;
    descriptor->base_mid = (base >> 16) & 0xff;
    descriptor->attributes = attributes & 0xff;
    descriptor->limit_high_and_attributes = ((limit >> 16) & 0x0f) | ((attributes >> 8) & 0xf0);
    descriptor->base_high = (base >> 24) & 0x0ff;
}


u32 segment_to_linear_addr(u16 selector) {
    Descriptor *entry = &gdt[selector >> 3];
    return ((entry->base_high & 0x0ff) << 24) | ((entry->base_mid & 0x0ff) << 16) | (entry->base_low & 0x0ffff);
}


u32 virtual_to_linear_addr(u16 selector, u32 offset) {
    return segment_to_linear_addr(selector) + offset;
}

