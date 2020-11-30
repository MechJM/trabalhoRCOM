#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

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
    char * at_pos = strstr(argv[1],"@");
    int user_length;
    int password_length;
 
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
    }

    return 0;
}