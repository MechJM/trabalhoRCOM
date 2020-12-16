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

int open_tcp_connection(char * ip_address, int port, int check_reply)
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
	if (check_reply)
	{
		char * reply;
		reply = read_reply(sockfd);
		free(reply);
	}
	

    return sockfd;
}

int close_tcp_connection(int sockfd)
{
   return close(sockfd);
}

int login_user(int sockfd, char * user, char * password)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));

	char user_message[MAX_STR_LEN];
	char pass_message[MAX_STR_LEN];

	sprintf(user_message, "user %s\n", user);
	sprintf(pass_message, "pass %s\n", password);

	int code1 = write_and_get_reply(sockfd, user_message, reply);

	if (code1 != 331)
	{
		fprintf(stderr, "Couldn't login as %s! Code received: %d\n", user, code1);
		return code1;
	}

	int code2 = write_and_get_reply(sockfd, pass_message, reply);

	if (code2 != 230)
	{
		fprintf(stderr, "Couldn't setup %s password! Code received: %d\n", user, code2);
		return code2;
	}

	free(reply);

	return 0;
}

int login_anonymous(int sockfd)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));
	char * reply2 = calloc(MAX_STR_LEN, sizeof(char));
	char user_message[] = "user anonymous\n";
	char pass_message[] = "pass random_string\n";

	int code = write_and_get_reply(sockfd, user_message, reply);
	//printf("Reply: %s\n",reply);

	if (code == 230)
	{
		free(reply);
		free(reply2);
		return 0;
	}
	else if (code == 331)
	{
		int code2 = write_and_get_reply(sockfd, pass_message, reply2);
		//printf("Reply2: %s\n",reply2);
	
		if (code2 != 230)
		{
			fprintf(stderr, "Couldn't setup anonymous user! Code received: %d\n", code);
			free(reply);
			free(reply2);
			return code;
		}
	}

	free(reply);
	free(reply2);

	return 0;
}

int enter_passive_get_port(int sockfd)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));
	char passive_cmd[] = "pasv\n";
	char sec2lastport_str[FTP_CODE_LENGTH + 1], lastport_str[FTP_CODE_LENGTH + 1];
	int sec2lastport, lastport;

	int code = write_and_get_reply(sockfd, passive_cmd, reply);

	if (code != 227)
	{
		fprintf(stderr, "Couldn't enter passive mode! Code received: %d\n",code);
		return -1;
	}

	int i = 0;
	int comma_counter = 0;
	int fourth_comma = -1, fifth_comma = -1;
	while (reply[i] != ')')
	{
		if (reply[i] == ',') comma_counter++;
		if (comma_counter == 4 && reply[i] == ',')
		{
			fourth_comma = i;
		}
		if (comma_counter == 5 && reply[i] == ',')
		{
			fifth_comma = i;
		}
		i++;
	}
	
	strncpy(sec2lastport_str, &reply[fourth_comma + 1], fifth_comma - fourth_comma - 1);
	sec2lastport = atoi(sec2lastport_str);

	strncpy(lastport_str, &reply[fifth_comma + 1], i - fifth_comma - 1);
	lastport = atoi(lastport_str);
	
	free(reply);
	/*
	printf("second 2 last str: %s\n",sec2lastport_str);
	printf("second 2 last: %d\n",sec2lastport);
	printf("last str: %s\n",lastport_str);
	printf("last: %d\n",lastport);
	*/
	return 256 * sec2lastport + lastport;
}

char * read_reply(int sockfd)
{
	char current_char[2];
	char * result = calloc(MAX_REPLY_SIZE, sizeof(char));
	char * line = calloc(MAX_STR_LEN, sizeof(char));
	char * first_four_chars = calloc(4 + 1, sizeof(char));

	while (1)
	{
		while (1)
		{
			read(sockfd, &current_char, 1);
			current_char[1] = 0;
			if (strcmp(current_char,"\n") == 0) break;
			else strcat(line, current_char);
		}
		strcat(line,"\n");
		strncpy(first_four_chars, line, 4);
		first_four_chars[4] = 0;
		strcat(result, line);
		//printf("Line: %s\n",line);
		if (first_four_chars[3] == ' '){
			strcpy(result, line);
			break;
		} 
		strcpy(line, "");
	}
	//printf("Reply: %s\n",result);
	free(line);
	free(first_four_chars);

	return result;
}

int request_file(char * file_path, int sockfd)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));
	

	char file_request_msg[MAX_STR_LEN];
	sprintf(file_request_msg, "retr %s\n", file_path);

	int code = write_and_get_reply(sockfd, file_request_msg,reply);

	if (code != 150)
	{
		fprintf(stderr, "Failed to request file! Code received: %d\n",code);
		return code;
	}

	free(reply);
	return 0;
}

unsigned char * receive_file(int sockfd, int sockfd2, int size)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));
	char current_char;
	unsigned char * result = calloc(size, sizeof(char));

	FILE * sockptr = fdopen(sockfd, "r");
	
	int i = 0;

	while(i < size)
	{
		current_char = fgetc(sockptr);
		if (current_char == EOF) break;
		else
		{
			result[i++] = (unsigned char) current_char;
		} 
	}

	reply = read_reply(sockfd2);
	
	if (strncmp(reply, "226", FTP_CODE_LENGTH) != 0)
	{
		fprintf(stderr, "Failed to transfer file! Reply received:\n%s\n",reply);
	}
	
	free(reply);

	return result;
}

int get_file_size(int sockfd, char * file_path)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));
	char file_size_msg[MAX_STR_LEN];

	sprintf(file_size_msg, "size %s\n", file_path);

	int code = write_and_get_reply(sockfd, file_size_msg, reply);

	if (code != 213)
	{
		fprintf(stderr, "Couldn't get file size! Code received: %d\n",code);
		return -1;
	}

	int result;

	sscanf(reply, "213 %d\n",&result);

	free(reply);

	return result;
}

int write_and_get_reply(int sockfd, char * msg, char * reply)
{
	char * reply_before_returning = calloc(MAX_STR_LEN, sizeof(char));

	char code[FTP_CODE_LENGTH + 1];

	write(sockfd, msg, strlen(msg));

	reply_before_returning = read_reply(sockfd);

	strcpy(reply, reply_before_returning);

	strncpy(code, reply, FTP_CODE_LENGTH);
	code[FTP_CODE_LENGTH] = 0;

	free(reply_before_returning);

	return atoi(code);
}

int change_transfer_mode(int sockfd, char * mode)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));

	char mode_change_request[MAX_STR_LEN];

	sprintf(mode_change_request, "type %s\n", mode);

	int code = write_and_get_reply(sockfd, mode_change_request, reply);

	if (code != 200)
	{
		fprintf(stderr,"Failed to change transfer mode! Code received: %d\n", code);
		return code;
	}

	free(reply);

	return 0;
}

int end_ftp_connection(int sockfd)
{
	char * reply = calloc(MAX_STR_LEN, sizeof(char));
	
	char termination_request[] = "quit\n";

	int code = write_and_get_reply(sockfd, termination_request, reply);
	
	if (code != 221)
	{
		fprintf(stderr, "Failed to terminate FTP connection! Code received: %d\n",code);
		return code;
	}

	free(reply);
	return 0;
}