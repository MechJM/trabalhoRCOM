#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "tram.h"
#include "state_machine.h"

#define MAX_TRAM_SIZE 2
#define BAUDRATE B38400

// Flag values
#define TRANSMITTER 0
#define RECEIVER 1

typedef struct
{
  char *port;
  int baudRate;
  unsigned int sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  char frame[MAX_TRAM_SIZE];
} link_layer;

static link_layer *ll;

int ll_init(char *port, int baudRate, unsigned int timeout, unsigned int numTransmissions);

int ll_open_serial_port(int fd);

int llopen(int fd, int flag);

int llwrite(int fd, char *buffer, int length);

int llread(int fd, char * buffer);

void ll_close_serial_port(int fd);

int llclose(int fd);