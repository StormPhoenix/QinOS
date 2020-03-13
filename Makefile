include Makefile.variable

BOOT 		= boot.bin
LOADER		= loader.bin
KERNEL		= kernel.bin

.PHONY: all clean

# Default
all: $(BOOT) $(LOADER) $(KERNEL)
	cp ./boot_backup.img ./boot.img
	dd if=boot.bin of=./boot.img bs=512 count=1 conv=notrunc
	$(MOUNT)
	$(CP_LOADER)
	$(CP_KERNEL)
	$(UMOUNT)
	rm -f $(BOOT)
	rm -f $(LOADER)
	rm -f $(KERNEL)

# Clean output files
clean:
	@make clean -C boot
	@make clean -C kernel
	rm -f $(BOOT)
	rm -f $(KERNEL)

boot.bin:
	@make boot.bin -C boot
	mv ./boot/boot.bin ./

loader.bin:
	@make loader.bin -C boot
	mv ./boot/loader.bin ./

kernel.bin:
	@make -C kernel
	mv ./kernel/kernel.bin ./