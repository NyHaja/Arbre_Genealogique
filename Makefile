CC = gcc
CFLAGS = -Wall -Wextra

canevas : canevas.c
	$(CC) $(CFLAGS) canevas.c -o canevas

run: canevas
	./canevas

clean :
	rm *.o prog canevas