include ../Makefile.variable

CFLAGS		= -I ../include -c -fno-builtin

OBJS		= kernel.o string_s.o string.o bits.o main.o traps.o interrupt.o pm.o sched.o sys_call.o console.o keyboard.o

TARGET		= kernel.bin

all: $(TARGET)
	rm -f $(OBJS)

clean: 
	rm -f $(OBJS)
	rm -f $(TARGET)

kernel.bin: $(OBJS)
	$(LD) -m elf_i386 -s -Ttext 0x30400 $(OBJS) -o $@

kernel.o: kernel.asm
	$(AS) -f elf kernel.asm -o $@

string.o: ../lib/string.c
	$(CC) -m32 $(CFLAGS) ../lib/string.c -o $@

string_s.o: ../lib/asm/string.asm
	$(AS) -f elf ../lib/asm/string.asm -o $@

bits.o: ../lib/asm/bits.asm
	$(AS) -f elf ../lib/asm/bits.asm -o $@

main.o: ../init/main.c
	$(CC) -m32 $(CFLAGS) ../init/main.c -o $@

interrupt.o: ./intr/interrupt.asm
	$(AS) -f elf ./intr/interrupt.asm -o $@

traps.o: ./intr/traps.c
	$(CC) -m32 $(CFLAGS) ./intr/traps.c -o $@

pm.o: ./pm.c
	$(CC) -m32 $(CFLAGS) ./pm.c -o $@

sched.o:
	$(CC) -m32 $(CFLAGS) ./task/sched.c -o $@

sys_call.o: ./task/sys_call.asm
	$(AS) -f elf ./task/sys_call.asm -o $@

console.o: ./console.c ../include/global.h
	$(CC) -m32 $(CFLAGS) ./console.c -o $@

keyboard.o: ./keyboard.c
	$(CC) -m32 $(CFLAGS) ./keyboard.c -o $@
