main: main.o listdir.o
	gcc -ansi -Wpedantic -o main main.o listdir.o

main.o: main.c listdir.h
	gcc -ansi -Wpedantic -c main.c

listdir.o: listdir.h
	gcc -ansi -Wpedantic -c listdir.c

clean: 
	rm *.o main