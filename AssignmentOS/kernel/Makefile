PREFIX=/media/disk.img
CFLAGS=-ffreestanding -nostdlib -I./include $(EXTRAFLAGS)
LINKFLAGS=-T link.ld -ffreestanding -nostdlib -lgcc $(EXTRAFLAGS)
NASM=nasm
NASMFLAGS=-f elf

C_SOURCES=*.c
SRC_DIRS=boot interrupts mm drivers scheduler utils programs

kernel.gz: kernel.bin
	gzip -c -9 $^ > $@

kernel.bin: main.o shell.o programs.o link.ld submodules
	$(CC) $$(find . -name '*.o' | grep -v '^\./programs/') -o $@ $(LINKFLAGS)

submodules:
	for dir in $(SRC_DIRS); do\
		cd $${dir};\
		make;\
		make_result=$$?;\
		if [ $$make_result -ne 0 ]; then\
		    exit $$make_result;\
		fi;\
		cd ..;\
	done

depend:
	-for dir in $(SRC_DIRS); do\
		cd $${dir};\
		make depend;\
		cd ..;\
	done
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

install: kernel.gz
	cp $^ $(PREFIX)/boot
	cd programs; make install; cd ..;
	sync

clean:
	-rm -rf */*.o
	-rm -rf *.o
	-rm -rf *.bin
	-rm -rf *.gz
	cd programs; make clean; cd ..;

main.o: main.c include/kernel.h include/tty.h include/constants.h  include/scheduler.h include/syscalls.h include/kernel.h include/timer.h  include/interrupts.h include/keyboard.h include/utils.h include/mm.h  include/tty.h include/syscalls.h include/floppy.h
shell.o: shell.c include/tty.h include/constants.h include/mm.h  include/kernel.h include/tty.h include/utils.h include/scheduler.h  include/syscalls.h include/sys.h include/fs.h
programs.o: programs.c include/tty.h include/constants.h include/utils.h  include/scheduler.h include/syscalls.h include/kernel.h include/tty.h  include/timer.h include/sys.h include/fs.h
