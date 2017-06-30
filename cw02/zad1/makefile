test: files.c bubbleSort.c
	gcc -o test files.c bubbleSort.c -I.
	./test LIB generate filename_lib 16384 256
	cp filename_lib ./filename_sys
	./test SYS sort filename_sys 700 4 > results.txt
	./test SYS shuffle filename_sys 700 4 >> results.txt
	./test SYS sort filename_sys 3000 4 >> results.txt
	./test SYS shuffle filename_sys 3000 4 >> results.txt
	./test LIB sort filename_lib 700 4 >> results.txt
	./test LIB shuffle filename_lib 700 4 >> results.txt
	./test LIB sort filename_lib 3000 4 >> results.txt
	./test LIB shuffle filename_lib 3000 4 >> results.txt

	./test SYS sort filename_sys 700 512 >> results.txt
	./test SYS shuffle filename_sys 700 512 >> results.txt
	./test SYS sort filename_sys 3000 512 >> results.txt
	./test SYS shuffle filename_sys 3000 512 >> results.txt
	./test LIB sort filename_lib 700 512 >> results.txt
	./test LIB shuffle filename_lib 700 512 >> results.txt
	./test LIB sort filename_lib 3000 512 >> results.txt
	./test LIB shuffle filename_lib 3000 512 >> results.txt

	./test SYS sort filename_sys 700 4096 >> results.txt
	./test SYS shuffle filename_sys 700 4096 >> results.txt
	./test SYS sort filename_sys 3000 4096 >> results.txt
	./test SYS shuffle filename_sys 3000 4096 >> results.txt
	./test LIB sort filename_lib 700 4096 >> results.txt
	./test LIB shuffle filename_lib 700 4096 >> results.txt
	./test LIB sort filename_lib 3000 4096 >> results.txt
	./test LIB shuffle filename_lib 3000 4096 >> results.txt

	./test SYS sort filename_sys 512 8192 >> results.txt
	./test SYS shuffle filename_sys 512 8192 >> results.txt
	./test SYS sort filename_sys 350 8192 >> results.txt
	./test SYS shuffle filename_sys 350 8192 >> results.txt
	./test LIB sort filename_lib 512 8192 >> results.txt
	./test LIB shuffle filename_lib 512 8192 >> results.txt
	./test LIB sort filename_lib 350 8192 >> results.txt
	./test LIB shuffle filename_lib 350 8192 >> results.txt

	
cleanTest:
	rm test filename_sys filename_lib

