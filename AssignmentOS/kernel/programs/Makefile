PREFIX=/media/disk.img
ENTRYPOINT=0x40000000
CFLAGS=-ffreestanding -nostdlib -I../include
LDFLAGS=-ffreestanding -nostdlib -Wl,--oformat=binary -Wl,-Ttext=$(ENTRYPOINT) -lgcc

C_SOURCES=*.c
PROGRAMS=help p1 p2 p3 p4 fork

all: $(PROGRAMS)

install: $(PROGRAMS)
	cp $^ $(PREFIX)

clean:
	-rm -rf $(PROGRAMS)

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

%: crt.o %.o
	$(CC) $^ -o $@ $(LDFLAGS)

help.o: help.c ../include/sys.h
sem.o: sem.c ../include/sys.h
p1.o: p1.c sleep_test.h ../include/sys.h
p2.o: p2.c sleep_test.h ../include/sys.h
p3.o: p3.c sleep_test.h ../include/sys.h
p4.o: p4.c sleep_test.h ../include/sys.h
fork.o: fork.c ../include/sys.h
crt.o: crt.c
