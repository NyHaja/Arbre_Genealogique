CC = gcc
CFLAGS =-Wall -Wextra

canevas : canevas.c
	$(CC) $(CFLAGS) canevas.c -o prog

run: canevas
	./prog

clean :
	rm *.o prog