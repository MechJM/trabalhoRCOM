#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "tcp_ip.h"

#define MAX_STR_LEN 200

int main(int argc, char * argv[])
{   
    
    if (argc < 2)
    {
        fprintf(stderr, "Usage: download <url>\n");
        exit(1);
    }
    
    char user[MAX_STR_LEN];
    char password[MAX_STR_LEN];
    char host[MAX_STR_LEN];
    char * at_pos = strstr(argv[1],"@");
    int user_length;
    int password_length;
    char * host_start = &argv[1][6];
    
    if (at_pos != NULL)
    {
        char * second_colon_pos = strstr(&argv[1][4],":");
        uintptr_t at_pos_value = (uintptr_t) at_pos;
        uintptr_t second_colon_pos_value = (uintptr_t) second_colon_pos;
        uintptr_t seventh_char_pos_value = (uintptr_t) &argv[1][6];
       
        password_length = at_pos_value - second_colon_pos_value - 1;
        strncpy(password, second_colon_pos + 1, password_length);
        
        user_length = second_colon_pos_value - seventh_char_pos_value;
        strncpy(user, &argv[1][6], user_length);

        host_start = at_pos + 1;
    }
    
    char * third_slash_pos = strstr(&argv[1][6],"/"); //assuming user, password and host don't contain slashes themselves
    uintptr_t third_slash_pos_value = (uintptr_t) third_slash_pos;

    uintptr_t host_start_value = (uintptr_t) host_start;
 
    int host_length = third_slash_pos_value - host_start_value;

    strncpy(host, host_start, host_length);
     
    char * ip_address = getIP(host);

    int sockfd = open_tcp_connection(ip_address, FTP_PORT);

    char reply[MAX_STR_LEN];

    read(sockfd,reply,MAX_STR_LEN);

    printf("Reply: %s\n",reply);

    char user_setup_message[] = "user anonymous\n";
    //char password_setup_message = "pass password\n";

    write(sockfd, user_setup_message, strlen(user_setup_message));

    read(sockfd,reply,MAX_STR_LEN);

    printf("Reply: %s\n",reply);

    close_tcp_connection(sockfd);

    return 0;
}