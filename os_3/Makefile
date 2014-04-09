LD=/usr/local/osos/bin/i386-osos-elf-ld
LDFLAGS=-N
AS=nasm
ASFLAGS=
CC=/usr/local/osos/bin/i386-osos-elf-gcc
CFLAGS=-mpreferred-stack-boundary=2 -ffreestanding -g3


all: program_list_0 user_program disk.img
	dd if=program_list_0 of=disk.img seek=64 conv=notrunc
	printf '\x40' | dd of=disk.img bs=1 seek=446 count=1 conv=notrunc
	dd if=help.com of=disk.img seek=65 conv=notrunc
	dd if=ls.com of=disk.img seek=68 conv=notrunc
	dd if=time.com of=disk.img seek=71 conv=notrunc
	dd if=date.com of=disk.img seek=74 conv=notrunc
	dd if=reboot.com of=disk.img seek=77 conv=notrunc
	dd if=read_write.com of=disk.img seek=78 conv=notrunc
	dd if=unixtime.com of=disk.img seek=79 conv=notrunc

disk.img: os.bin loader.bin
	dd if=/dev/zero of=disk.img count=2880
	dd if=loader.bin of=disk.img conv=notrunc
	dd if=os.bin of=disk.img seek=1 conv=notrunc
	printf '\x55\xaa' | dd of=disk.img bs=1 seek=510 count=2 conv=notrunc


loader.bin: loader.o utils.o
	$(LD) $(LDFLAGS) -Ttext 0x7c00 --oformat binary -o $@ $^

os.bin: os.o syscalls.o int_handler_wrappers.o utils_32cc.o
	$(LD) $(LDFLAGS) -Ttext 0x0500 --oformat binary -o $@ $^

user_program: help.com ls.com time.com date.com reboot.com read_write.com unixtime.com


#dependencies
os.o: os.c os.h utils_32cc.h int_handler_wrappers.h
syscalls.o: syscalls.c syscalls.h utils_32cc.h os.h
help.com: help.o utils_32cc.o
help.o: help.c utils_32cc.h user_program.h
ls.com: ls.o utils_32cc.o
ls.o: ls.c utils_32cc.h user_program.h
time.com: time.o utils_32cc.o
time.o: time.c utils_32cc.h user_program.h
date.com: date.o utils_32cc.o
date.o: date.c utils_32cc.h user_program.h
unixtime.o: unixtime.c syscalls.h user_program.h os.h


#rules
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.nasm
	$(AS) $(ASFLAGS) -f elf32 -o $@ $^

%.com: %.o
	$(LD) $(LDFLAGS) -Ttext 0x0100 --oformat binary -o $@ $^


#special compile (e.g. pure asm user program)
reboot.com: reboot.nasm
	$(AS) $(ASFLAGS) -o $@ $^

read_write.com: read_write.nasm
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	-rm -f disk.img
	-rm -f *.o
	-rm -f *.bin
	-rm -f *.com
	-rm -f *.gch
