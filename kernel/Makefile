include ../Makefile.variable

CFLAGS		= -I ../include -c

OBJS		= kernel.o string.o main.o

TARGET		= kernel.bin

all: $(TARGET)

clean: 
	rm -f $(OBJS)
	rm -f $(TARGET)

kernel.bin: $(OBJS)
	$(LD) -m elf_i386 -s -Ttext 0x30400 $(OBJS) -o $@

kernel.o: kernel.asm
	$(AS) -f elf kernel.asm -o $@

string.o: ../lib/string.asm
	$(AS) -f elf ../lib/string.asm -o $@

main.o: main.c 
	$(CC) -m32 $(CFLAGS) main.c -o $@

