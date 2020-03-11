#include "pm.h"
#include "string.h"


u32 segment_to_linear_addr(u16 selector) {
    Descriptor *entry = &gdt[selector >> 3];
    return ((entry->base_high & 0x0ff) << 24) | ((entry->base_mid & 0x0ff) << 16) | (entry->base_low & 0x0ffff);
}


u32 virtual_to_linear_addr(u16 selector, u32 offset) {
    return segment_to_linear_addr(selector) + offset;
}

