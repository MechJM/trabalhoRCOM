#pragma once

#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>

#define FTP_PORT 21

char * getIP(char * hostname);

int open_tcp_connection(char * ip_address, int port);

int close_tcp_connection(int sockfd);