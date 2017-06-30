all:
	gcc barber.c my_queue.c -o barber -pthread -lrt
	gcc clients.c my_queue.c -o clients -pthread -lrt
clean:
	rm -f barber clients
