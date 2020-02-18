include ../Makefile.variable

AS_FLAGS	= -I include/

TARGET		= boot.bin loader.bin

.PHONY: all clean

# Default
all: $(TARGET)

# Clean outputs
clean:
	rm -f $(TARGET)

boot.bin: boot.asm include/fat12.inc
	$(AS) $(AS_FLAGS) -o $@ $<

boot: boot.bin

loader.bin: loader.asm include/fat12.inc include/pm.inc include/boot.inc
	$(AS) $(AS_FLAGS) -o $@ $<

loader: loader.bin