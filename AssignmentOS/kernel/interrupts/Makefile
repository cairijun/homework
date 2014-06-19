CFLAGS=-ffreestanding -nostdlib -I../include $(EXTRAFLAGS)
LINKFLAGS=-T link.ld -ffreestanding -nostdlib -lgcc $(EXTRAFLAGS)
NASM=nasm
NASMFLAGS=-f elf

C_SOURCES=*.c

all: int_handlers.o idt.o pic.o keyboard.o timer.o syscalls.o

int_handlers.o: int_handlers.asm

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

idt.o: idt.c ../include/kernel.h ../include/tty.h ../include/constants.h  ../include/interrupts.h ../include/kernel.h ../include/keyboard.h
keyboard.o: keyboard.c ../include/utils.h ../include/interrupts.h  ../include/kernel.h ../include/tty.h ../include/constants.h  ../include/keyboard.h ../include/scheduler.h ../include/syscalls.h  ../include/keyboard.h ../include/keyboard_map.h
pic.o: pic.c ../include/interrupts.h ../include/kernel.h ../include/tty.h  ../include/constants.h ../include/keyboard.h ../include/utils.h
timer.o: timer.c ../include/timer.h ../include/constants.h  ../include/utils.h ../include/scheduler.h ../include/syscalls.h  ../include/kernel.h ../include/tty.h ../include/interrupts.h  ../include/keyboard.h ../include/constants.h
syscalls.o: syscalls.c ../include/syscalls.h ../include/constants.h  ../include/scheduler.h ../include/constants.h ../include/syscalls.h  ../include/kernel.h ../include/tty.h ../include/interrupts.h  ../include/keyboard.h ../include/timer.h ../include/tty.h
