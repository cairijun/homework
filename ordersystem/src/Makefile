OBJECT = main.o MenuClass.o Menu.o User.o OrdersManager.o CSVParser.o
DEBUG = -Wall
FLAGS = -c -O2 $(DEBUG)
CC = c++

ordersystem: $(OBJECT)
	$(CC) $(OBJECT) -o $@
	strip $@

main.o: main.cpp User.h OrdersManager.h Menu.h MenuClass.h CSVParser.h
	$(CC) $(FLAGS) -o $@ main.cpp

MenuClass.o: MenuClass.cpp MenuClass.h Menu.h CSVParser.h
	$(CC) $(FLAGS) -o $@ MenuClass.cpp

Menu.o: Menu.cpp Menu.h
	$(CC) $(FLAGS) -o $@ Menu.cpp

User.o: User.cpp User.h OrdersManager.h Menu.h MenuClass.h CSVParser.h
	$(CC) $(FLAGS) -o $@ User.cpp

OrdersManager.o: OrdersManager.cpp OrdersManager.h Menu.h MenuClass.h \
	CSVParser.h
	$(CC) $(FLAGS) -o $@ OrdersManager.cpp

CSVParser.o: CSVParser.cpp CSVParser.h
	$(CC) $(FLAGS) -o $@ CSVParser.cpp

clean:
	-rm -f ordersystem $(OBJECT) *~

