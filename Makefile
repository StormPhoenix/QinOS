include Makefile.variable

BOOT 		= boot/boot.bin boot/loader.bin
KERNEL		= kernel/kernel.bin

.PHONY: all clean

# Default
all: $(BOOT) $(KERNEL)

# build image
build: all
	cp ./boot_backup.img ./boot.img
	dd if=boot/boot.bin of=./boot.img bs=512 count=1 conv=notrunc
	mount -o loop ./boot.img /mnt/floppy
	cp  boot/loader.bin /mnt/floppy
	cp 	kernel/kernel.bin /mnt/floppy
	umount /mnt/floppy

# Clean output files
clean:
	@make clean -C boot
	@make clean -C kernel

boot/boot.bin: 
	@make boot -C boot

boot/loader.bin:
	@make loader -C boot

kernel/kernel.bin:	
	@make -C kernel