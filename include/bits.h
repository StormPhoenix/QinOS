#ifndef _BITS_H_
#define _BITS_H_

#include "type.h"

void out_byte(int port, u8 byte);

u8 in_byte(int port);

void enable_irq(int irq);

#endif