#include "tcp_ip.h"

char * getIP(char * hostname)
{
    struct hostent *h;    
        
/*
struct hostent {
	char    *h_name;	Official name of the host. 
    	char    **h_aliases;	A NULL-terminated array of alternate names for the host. 
	int     h_addrtype;	The type of address being returned; usually AF_INET.
    	int     h_length;	The length of the address in bytes.
	char    **h_addr_list;	A zero-terminated array of network addresses for the host. 
				Host addresses are in Network Byte Order. 
};

#define h_addr h_addr_list[0]	The first address in h_addr_list. 
*/
        if ((h=gethostbyname(hostname)) == NULL) {  
            herror("gethostbyname");
            exit(1);
        }
        /*
        printf("Host name  : %s\n", h->h_name);
        printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));
        */
        return inet_ntoa(*((struct in_addr *)h->h_addr));

}

int open_tcp_connection(char * ip_address, int port)
{
	int	sockfd;
	struct	sockaddr_in server_addr;
	//char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	//int	bytes;
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_address);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(sockfd, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		exit(0);
	}
    	/*send a string to the server*/
	//bytes = write(sockfd, buf, strlen(buf));
	//printf("Bytes escritos %d\n", bytes);

	char * reply;

	reply = read_reply(sockfd);

	free(reply);

    return sockfd;
}

int close_tcp_connection(int sockfd)
{
   return close(sockfd);
}

int login_rcom(int sockfd)
{
	char * reply;
	char code[FTP_CODE_LENGTH + 1];

	char user_message[] = "user rcom\n";
	char pass_message[] = "pass rcom\n";

	write(sockfd, user_message, strlen(user_message));

	reply = read_reply(sockfd);

	strncpy(code, reply, FTP_CODE_LENGTH);

	if (strcmp(code,"331") != 0)
	{
		fprintf(stderr, "Couldn't login as rcom! Code received: %s\n", code);
		return atoi(code);
	}

	write(sockfd, pass_message, strlen(pass_message));

	read(sockfd, reply, MAX_STR_LEN);

	strncpy(code, reply, FTP_CODE_LENGTH);

	if (strcmp(code, "230") != 0)
	{
		fprintf(stderr, "Couldn't setup rcom password! Code received: %s\n", code);
		return atoi(code);
	}

	free(reply);

	return 0;
}

int login_anonymous(int sockfd)
{
	char * reply;
	char code[FTP_CODE_LENGTH + 1];

	char user_message[] = "user anonymous\n";

	write(sockfd, user_message, strlen(user_message));

	reply = read_reply(sockfd);
	
	strncpy(code, reply, FTP_CODE_LENGTH);
	
	if (strcmp(code, "230") != 0)
	{
		fprintf(stderr, "Couldn't setup anonymous user! Code received: %s\n", code);
		return atoi(code);
	}

	free(reply);

	return 0;
}

int enter_passive_get_port(int sockfd)
{
	char * reply;
	char code[FTP_CODE_LENGTH + 1];
	char passive_cmd[] = "pasv\n";
	char sec2lastport_str[FTP_CODE_LENGTH + 1], lastport_str[FTP_CODE_LENGTH + 1];
	int sec2lastport, lastport;

	write(sockfd, passive_cmd, strlen(passive_cmd));

	reply = read_reply(sockfd);

	strncpy(code, reply, FTP_CODE_LENGTH);
	
	if (strcmp(code,"227") != 0)
	{
		fprintf(stderr, "Couldn't enter passive mode! Code received: %s\n",code);
		return -1;
	}

	strncpy(sec2lastport_str, reply + PORT_OFFSET1, FTP_CODE_LENGTH);
	sec2lastport = atoi(sec2lastport_str);

	strncpy(lastport_str, reply + PORT_OFFSET2, FTP_CODE_LENGTH);
	lastport = atoi(lastport_str);

	free(reply);

	return 256 * sec2lastport + lastport;
}

char * read_reply(int sockfd)
{
	char current_char[1];
	char * result = calloc(MAX_STR_LEN, sizeof(char));

	while(1)
	{
		read(sockfd, &current_char, 1);
		if (strcmp(current_char,"\n") == 0) break;
		else strcat(result, current_char);
	}

	return result;
}