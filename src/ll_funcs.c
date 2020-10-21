#include "ll_funcs.h"
#include "tram.h"
#include "state_machine.h"

int llclose(int fd)
{
    if (sender)
    {
        unsigned char * new_tram = generate_su_tram(COMM_SEND_REP_REC, DISC);
        int size = 5;
        byte_stuff(new_tram, &size);
        int res = write(fd, new_tram, size);
        if (res != 5) fprintf(stderr, "Failed to write on llclose!\n");
        unsigned char * response = receive_tram(DISC, fd);
        int response_size = 3;
        byte_unstuff(response, &response_size);
        struct parse_results * results = parse_tram(response, response_size);
        process_tram_received(results,fd);
    }
    return 1;
}