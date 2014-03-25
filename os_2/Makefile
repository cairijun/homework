LD=ld
LDFLAGS=-melf_i386 -N
AS=nasm
ASFLAGS=
CC=gcc
CFLAGS=-march=i386 -m32 -mpreferred-stack-boundary=2 -ffreestanding

disk.img: os.bin loader.bin
	dd if=/dev/zero of=disk.img count=2880
	dd if=loader.bin of=disk.img conv=notrunc
	dd if=os.bin of=disk.img seek=1 conv=notrunc
	printf '\x55\xaa' | dd of=disk.img bs=1 seek=510 count=2 conv=notrunc

os.bin: os.o utils_32cc.o
	$(LD) $(LDFLAGS) -Ttext 0x0500 --oformat binary -o $@ $^

loader.bin: loader.o utils.o
	$(LD) $(LDFLAGS) -Ttext 0x7c00 --oformat binary -o $@ $^

os.o: os.c os.h utils_32cc.h
	$(CC) $(CFLAGS) -c $^

loader.o: loader.nasm
	$(AS) $(ASFLAGS) -f elf32 -o $@ $^

utils.o: utils.nasm
	$(AS) $(ASFLAGS) -f elf32 -o $@ $^

utils_32cc.o: utils_32cc.nasm
	$(AS) $(ASFLAGS) -f elf32 -o $@ $^

all: program_list_0 help.com ls.com time.com date.com reboot.com disk.img
	dd if=program_list_0 of=disk.img seek=64 conv=notrunc
	printf '\x40' | dd of=disk.img bs=1 seek=446 count=1 conv=notrunc
	dd if=help.com of=disk.img seek=65 conv=notrunc
	dd if=ls.com of=disk.img seek=68 conv=notrunc
	dd if=time.com of=disk.img seek=71 conv=notrunc
	dd if=date.com of=disk.img seek=74 conv=notrunc
	dd if=reboot.com of=disk.img seek=77 conv=notrunc

user_program: help.com ls.com time.com date.com reboot.com

help.com: help.o utils_32cc.o
	$(LD) $(LDFLAGS) -Ttext 0x0100 --oformat binary -o $@ $^

help.o: help.c utils_32cc.h user_program.h
	$(CC) $(CFLAGS) -c $^

ls.com: ls.o utils_32cc.o
	$(LD) $(LDFLAGS) -Ttext 0x0100 --oformat binary -o $@ $^

ls.o: ls.c utils_32cc.h user_program.h
	$(CC) $(CFLAGS) -c $^

time.com: time.o utils_32cc.o
	$(LD) $(LDFLAGS) -Ttext 0x0100 --oformat binary -o $@ $^

time.o: time.c utils_32cc.h user_program.h
	$(CC) $(CFLAGS) -c $^

date.com: date.o utils_32cc.o
	$(LD) $(LDFLAGS) -Ttext 0x0100 --oformat binary -o $@ $^

date.o: date.c utils_32cc.h user_program.h
	$(CC) $(CFLAGS) -c $^

reboot.com: reboot.nasm
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	-rm disk.img
	-rm -f *.o
	-rm -f *.bin
	-rm -f *.com
	-rm -f *.gch
