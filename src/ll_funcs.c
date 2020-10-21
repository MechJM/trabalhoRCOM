#include "ll_funcs.h"
#include "tram.h"
#include "state_machine.h"

int llclose(int fd)
{
    if (sender)
    {
        unsigned char * new_tram = generate_su_tram(COMM_SEND_REP_REC, DISC);
        int size = NON_INFO_TRAM_SIZE;
        int res = write(fd, new_tram, size);
        if (res != NON_INFO_TRAM_SIZE)
        {
            fprintf(stderr, "Failed to write on llclose!\n");
            return -1;
        } 

        unsigned char * response = receive_tram(fd);
        int result = parse_and_process_su_tram(response, fd);
        if (result != DO_NOTHING)
        {
            fprintf(stderr, "Failed to write on llclose!\n");
            return -1;
        } 
    }
    else 
    {
        unsigned char * end_request = receive_tram(fd);
        int result = parse_and_process_su_tram(end_request, fd);
        if (result != DO_NOTHING)
        {
            fprintf(stderr, "Failed to write on llclose!\n");
            return -1;
        }

        unsigned char * acknowledgment = receive_tram(fd);
        result = parse_and_process_su_tram(acknowledgment, fd);
        if (result != DO_NOTHING)
        {
            fprintf(stderr, "Failed to write on llclose!\n");
            return -1;
        }
        
    }
    return 1;
}