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
    return sockfd;
}

int close_tcp_connection(int sockfd)
{
   return close(sockfd);
}