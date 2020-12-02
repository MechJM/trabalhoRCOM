CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -rdynamic #-g -O0
DOWNLOAD_OBJS=main.o tcp_ip.o

default: download lesser_clean

download: $(DOWNLOAD_OBJS)
	$(CC) $(CFLAGS) $(DOWNLOAD_OBJS) -o download

tcp_ip.o: tcp_ip.c tcp_ip.h
	$(CC) $(CFLAGS) -c tcp_ip.c

download.o: main.c
	$(CC) $(CFLAGS) -c main.c

lesser_clean:
	rm -rf *.o

clean:
	rm -rf *.o download