mytar: mytar.o listdir.o
	gcc -ansi -Wpedantic -o mytar mytar.o listdir.o

mytar.o: mytar.c listdir.h
	gcc -ansi -Wpedantic -c mytar.c

listdir.o: listdir.h
	gcc -ansi -Wpedantic -c listdir.c

clean: 
	rm *.o mytar