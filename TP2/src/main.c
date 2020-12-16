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

        uintptr_t seventh_char_pos_value = (uintptr_t) &argv[1][6];
        uintptr_t at_pos_value = (uintptr_t) at_pos;

        if (second_colon_pos != NULL)
        {
            uintptr_t second_colon_pos_value = (uintptr_t) second_colon_pos;
            
            password_length = at_pos_value - second_colon_pos_value - 1;
            strncpy(password, second_colon_pos + 1, password_length);
            password[password_length] = 0;
            
            user_length = second_colon_pos_value - seventh_char_pos_value;
            strncpy(user, &argv[1][6], user_length);
            user[user_length] = 0;
        }
        else
        {
            printf("Enter the password:\n");
            scanf("%s", password);

            user_length = at_pos_value - seventh_char_pos_value;
            strncpy(user, &argv[1][6], user_length);
            user[user_length] = 0;
        }
        
        host_start = at_pos + 1;
        
    }
    
    char * third_slash_pos = strstr(&argv[1][6],"/"); //assuming user, password and host don't contain slashes themselves
    uintptr_t third_slash_pos_value = (uintptr_t) third_slash_pos;
    uintptr_t host_start_value = (uintptr_t) host_start;
    int host_length = third_slash_pos_value - host_start_value;
    strncpy(host, host_start, host_length);
    strncpy(url_path, third_slash_pos + 1, MAX_STR_LEN);
    host[host_length] = 0;
    //Done parsing argv
    
    //Extracting filename
    int last_slash_index = -1;
    for (size_t i = 0; i < strlen(url_path); i++)
    {
        if (url_path[i] == '/') last_slash_index = i;
    }
    int filename_length = strlen(url_path) - last_slash_index - 1;
    strncpy(filename, &url_path[last_slash_index + 1], filename_length);
    filename[filename_length] = 0;
    //Done extracting filename
    
    char * ip_address = getIP(host);
    
    int sockfd = open_tcp_connection(ip_address, FTP_PORT, CHECK_REPLY);
       
    if (at_pos == NULL)
    {
        if (login_anonymous(sockfd))
        {
            fprintf(stderr, "Function has failed to login anonymously!\n");
            close_tcp_connection(sockfd);
            return 1;
        }
    }
    else
    {
        if (login_user(sockfd, user, password))
        {
            fprintf(stderr, "Function has failed to login with provided user!\n");
            close_tcp_connection(sockfd);
            return 1;
        }
    }
    
    
    
    int second_connection_port = enter_passive_get_port(sockfd);
    
    int sockfd2 = open_tcp_connection(ip_address, second_connection_port, DONT_CHECK_REPLY);

    int size = get_file_size(sockfd, url_path);

    if (change_transfer_mode(sockfd, "I"))
    {
        fprintf(stderr, "Couldn't change to binary mode!\n");
        close_tcp_connection(sockfd);
        close_tcp_connection(sockfd2);
        return 1;
    }

    if (request_file(url_path, sockfd))
    {
        fprintf(stderr, "Couldn't request file!\n");
        close_tcp_connection(sockfd);
        close_tcp_connection(sockfd2);
        return 1;
    }

    unsigned char * file = receive_file(sockfd2, sockfd, size);
 
    FILE * new_file = fopen(filename, "w+");

    fwrite(file, sizeof(unsigned char), size, new_file);    

    fclose(new_file);

    free(file);
    
    if (end_ftp_connection(sockfd))
    {
        fprintf(stderr, "Couldn't terminate connection!\n");
        close_tcp_connection(sockfd);
        close_tcp_connection(sockfd2);
        return 1;
    }
    
    close_tcp_connection(sockfd);

    close_tcp_connection(sockfd2);
    
    return 0;
}