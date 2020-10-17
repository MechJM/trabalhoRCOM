CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -rdynamic
WRITE_OBJECTS= writenoncanonical.o tram.o
READ_OBJECTS= noncanonical.o tram.o

default: read write lesser_clean

read: $(READ_OBJECTS)
	$(CC) $(CFLAGS) $(READ_OBJECTS) -o read

write: $(WRITE_OBJECTS)
	$(CC) $(CFLAGS) $(WRITE_OBJECTS) -o write

tram.o: tram.c tram.h
	$(CC) $(CFLAGS) -c tram.c
	
noncanonical.o:	noncanonical.c
	$(CC) $(CFLAGS) -c noncanonical.c

writenoncanonical.o: writenoncanonical.c
	$(CC) $(CFLAGS) -c writenoncanonical.c

lesser_clean:
	rm -rf *.o

clean:
	rm -rf *.o read write