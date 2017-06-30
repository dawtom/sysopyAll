static: main.c list.c
	gcc -Og -Wall -c list.c  -o list.o
	ar crs libsysopylab1.a list.o
	gcc -Og -Wall -static main.c -L. -lsysopylab1 -o listExecute
	./listExecute
shared:
	gcc -c -Wall -fPIC list.c -o list.o
	gcc -shared list.o -o libsysopy.so
	gcc main.c -Wall -o main -L. -lsysopy
	export LD_LIBRARY_PATH=.;./main
dynamic:
	gcc -c -Wall -fPIC list.c -o list.o
	gcc -shared list.o -o libsysopy.so
	gcc -Wall -L. ./mainDynamic.c -lsysopy -o ./main_dynamic -ldl
	export LD_LIBRARY_PATH=.;./main_dynamic
	
	
cleanStatic:
	rm list.o
	rm listExecute
	rm libsysopylab1.a

cleanShared:
	rm main
	rm list.o
	rm libsysopy.so

cleanDynamic:
	rm list.o
	rm libsysopy.so
	rm main_dynamic
