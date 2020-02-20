include ../Makefile.variable

CFLAGS		= -I ../include -c

OBJS		= kernel.o string.o bits.o main.o trap.o

TARGET		= kernel.bin

all: $(TARGET)

clean: 
	rm -f $(OBJS)
	rm -f $(TARGET)

kernel.bin: $(OBJS)
	$(LD) -m elf_i386 -s -Ttext 0x30400 $(OBJS) -o $@

kernel.o: kernel.asm
	$(AS) -f elf kernel.asm -o $@

string.o: ../lib/asm/string.asm
	$(AS) -f elf ../lib/asm/string.asm -o $@

bits.o: ../lib/asm/bits.asm
	$(AS) -f elf ../lib/asm/bits.asm -o $@

main.o: main.c
	$(CC) -m32 $(CFLAGS) main.c -o $@

trap.o: trap.c
	$(CC) -m32 $(CFLAGS) trap.c -o $@