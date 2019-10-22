frecpal: frecpal.o list.o
	gcc -ansi -Wpedantic -o frecpal frecpal.o list.o

frecpal.o: frecpal.c list.h
	gcc -ansi -Wpedantic -c frecpal.c

list.o: list.h
	gcc -ansi -Wpedantic -c list.c

clean: 
	rm *.o frecpal