CFLAGS=-ffreestanding -nostdlib -I../include $(EXTRAFLAGS)
LINKFLAGS=-T link.ld -ffreestanding -nostdlib -lgcc $(EXTRAFLAGS)
NASM=nasm
NASMFLAGS=-f elf

C_SOURCES=*.c

all: tty.o keyboard_map.o floppy.o fat12.o fs.o

depend:
	$(CC) $(C_SOURCES) -MM $(CFLAGS) | while read row; do\
		e_row=$${row//\//\\/};\
		e_row=$${e_row//./\\.};\
		sed -i "s/^$${e_row%:*}.*$$/$${row//\//\\/}/" Makefile;\
		grep -q "$$e_row" Makefile;\
		if [ $$? -eq 1 ]; then\
			echo $${row} >> Makefile;\
		fi;\
	done;

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.asm
	$(NASM) $^ -o $@ $(NASMFLAGS)

keyboard_map.o: keyboard_map.c ../include/keyboard_map.h
tty.o: tty.c ../include/constants.h ../include/tty.h  ../include/constants.h ../include/keyboard.h ../include/keyboard_map.h  ../include/scheduler.h ../include/syscalls.h ../include/kernel.h  ../include/tty.h ../include/interrupts.h ../include/keyboard.h  ../include/utils.h
floppy.o: floppy.c ../include/scheduler.h ../include/constants.h  ../include/syscalls.h ../include/kernel.h ../include/tty.h  ../include/constants.h ../include/interrupts.h ../include/keyboard.h  ../include/floppy.h ../include/utils.h
fat12.o: fat12.c ../include/fs.h ../include/floppy.h  ../include/scheduler.h ../include/constants.h ../include/syscalls.h  ../include/kernel.h ../include/tty.h ../include/utils.h ../include/mm.h
fs.o: fs.c ../include/fs.h ../include/utils.h
