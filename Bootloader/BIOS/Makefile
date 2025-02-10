boot.img: bootloader.asm
	nasm $^ -f bin -o $@

test: boot.img
	qemu-system-i386 -hda boot.img