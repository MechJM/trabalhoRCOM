CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -rdynamic #-g -O0
DOWNLOAD_OBJS= main.o

default: download lesser_clean

download: $(DOWNLOAD_OBJS)
	$(CC) $(CFLAGS) $(DOWNLOAD_OBJS) -o download

download.o: main.c
	$(CC) $(CFLAGS) -c main.c

lesser_clean:
	rm -rf *.o

clean:
	rm -rf *.o download