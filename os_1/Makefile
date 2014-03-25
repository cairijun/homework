LD=ld
LDFLAGS=
AS=nasm
ASFLAGS=

disk.img: os.bin loader.bin
	dd if=/dev/zero of=disk.img count=2880
	dd if=loader.bin of=disk.img conv=notrunc
	dd if=os.bin of=disk.img seek=1 conv=notrunc
	printf '\x55\xaa' | dd of=disk.img bs=1 seek=510 count=2 conv=notrunc

os.bin: os.o utils.o
	$(LD) $(LDFLAGS) -Ttext 0 --oformat binary -o $@ $^

loader.bin: loader.o utils.o
	$(LD) $(LDFLAGS) -Ttext 0 --oformat binary -o $@ $^

os.o: os.nasm
	$(AS) $(ASFLAGS) -f elf64 -o $@ $^

loader.o: loader.nasm
	$(AS) $(ASFLAGS) -f elf64 -o $@ $^

utils.o: utils.nasm
	$(AS) $(ASFLAGS) -f elf64 -o $@ $^

user_programs: help.com ls.com reboot.com stone.com program_list disk.img
	dd if=help.com of=disk.img seek=3 conv=notrunc
	dd if=ls.com of=disk.img seek=4 conv=notrunc
	dd if=reboot.com of=disk.img seek=5 conv=notrunc
	dd if=stone.com of=disk.img seek=6 conv=notrunc
	dd if=program_list of=disk.img bs=1 seek=446 conv=notrunc

help.com: help.nasm
	$(AS) $(ASFLAGS) -o $@ $^

ls.com: ls.nasm
	$(AS) $(ASFLAGS) -o $@ $^

reboot.com: reboot.nasm
	$(AS) $(ASFLAGS) -o $@ $^

stone.com: stone.nasm
	$(AS) $(ASFLAGS) -o $@ $^

clean:
	-rm disk.img
	-rm -f *.o
	-rm -f *.bin
	-rm -f *.com
