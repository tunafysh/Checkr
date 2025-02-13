all:
	nasm bootloader.asm -f bin -o stage1.bin
	nasm stage2.asm -f bin -o stage2.bin
	cat stage1.bin stage2.bin > boot.bin

test: boot.bin
	qemu-system-i386 -hda boot.bin