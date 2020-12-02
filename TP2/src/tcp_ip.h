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
#include <string.h>

#define FTP_PORT 21

#define MAX_STR_LEN 200

#define FTP_CODE_LENGTH 3

#define PORT_OFFSET1 43
#define PORT_OFFSET2 47

char * getIP(char * hostname);

int open_tcp_connection(char * ip_address, int port);

int close_tcp_connection(int sockfd);

int login_rcom(int sockfd);

int login_anonymous(int sockfd);

int enter_passive_get_port(int sockfd);

char * read_reply(int sockfd);