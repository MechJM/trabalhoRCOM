CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -rdynamic
WRITE_OBJECTS= writenoncanonical.o tram.o state_machine.o app_layer.o link_layer.o
READ_OBJECTS= noncanonical.o tram.o state_machine.o

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

state_machine.o: state_machine.c state_machine.h tram.h
	$(CC) $(CFLAGS) -c state_machine.c

app_layer.o: app_layer.c app_layer.h tram.h
	$(CC) $(CFLAGS) -c app_layer.c

link_layer.o: link_layer.c link_layer.h
	$(CC) $(CFLAGS) -c link_layer.c

lesser_clean:
	rm -rf *.o

clean:
	rm -rf *.o read write