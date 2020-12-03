#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "tcp_ip.h"

int main(int argc, char * argv[])
{   
    if (argc < 2)
    {
        fprintf(stderr, "Usage: download <url>\n");
        exit(1);
    }
    //Parsing argv

    //URL components
    char user[MAX_STR_LEN];
    char password[MAX_STR_LEN];
    char host[MAX_STR_LEN];
    char url_path[MAX_STR_LEN];
    char filename[MAX_STR_LEN];

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
    strncpy(url_path, third_slash_pos + 1, MAX_STR_LEN);
    //Done parsing argv

    //Extracting filename
    int last_slash_index = -1;
    for (size_t i = 0; i < strlen(url_path); i++)
    {
        if (url_path[i] == '/') last_slash_index = i;
    }
    int filename_length = strlen(url_path) - last_slash_index - 1;
    strncpy(filename, &url_path[last_slash_index + 1], filename_length);
    //Done extracting filename
    
    char * ip_address = getIP(host);
    
    int sockfd = open_tcp_connection(ip_address, FTP_PORT, CHECK_REPLY);
       
    /*
    if (login_rcom(sockfd))
    {
        fprintf(stderr, "Function has failed to login as rcom!\n");
    }
    */
    
    if (login_anonymous(sockfd))
    {
        fprintf(stderr, "Function has failed to login anonymously!\n");
    }

    int second_connection_port = enter_passive_get_port(sockfd);
    
    int sockfd2 = open_tcp_connection(ip_address, second_connection_port, DONT_CHECK_REPLY);

    int size = get_file_size(sockfd, url_path);

    if (request_file(url_path, sockfd))
    {
        fprintf(stderr, "Couldn't request file!\n");
    }

    char * file = receive_file(sockfd2, size);

    FILE * new_file = fopen(filename, "w+");

    fwrite(file, sizeof(char), size - 1, new_file);    

    fclose(new_file);

    free(file);

    close_tcp_connection(sockfd);

    close_tcp_connection(sockfd2);
    
    return 0;
}