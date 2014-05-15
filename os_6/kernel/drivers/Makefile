CFLAGS=-ffreestanding -nostdlib -I../include $(EXTRAFLAGS)
LINKFLAGS=-T link.ld -ffreestanding -nostdlib -lgcc $(EXTRAFLAGS)
NASM=nasm
NASMFLAGS=-f elf

C_SOURCES=*.c

all: tty.o keyboard_map.o

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
tty.o: tty.c ../include/constants.h ../include/tty.h  ../include/constants.h ../include/keyboard.h ../include/keyboard_map.h  ../include/scheduler.h ../include/kernel.h ../include/tty.h  ../include/utils.h
