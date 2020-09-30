CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -rdynamic

default: read write

read:	noncanonical.o
	$(CC) $(CFLAGS) noncanonical.o -o read

write: writenoncanonical.o
	$(CC) $(CFLAGS) writenoncanonical.o -o write
	
noncanonical.o:	noncanonical.c
	$(CC) $(CFLAGS) -c noncanonical.c

writenoncanonical.o:	writenoncanonical.c
	$(CC) $(CFLAGS) -c writenoncanonical.c

clean:
	rm -rf *.o read write



