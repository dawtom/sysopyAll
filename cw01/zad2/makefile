static: main.c list.c
	gcc -c -Wall list.c  -o list.o
	ar crs libsysopylab1.a list.o
	gcc -static -Wall main.c -L. -lsysopylab1 -o listExecute
	./listExecute
shared:
	gcc -c -Wall -fPIC list.c -o list.o
	gcc -shared list.o -o libsysopy.so
	gcc main.c -Wall -o main -L. -lsysopy
	export LD_LIBRARY_PATH=.
	./main
	
cleanStatic:
	rm list.o
	rm listExecute
	rm libsysopylab1.a

cleanShared:
	rm main
	rm list.o
	rm libsysopy.so
