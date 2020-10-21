#include "state_machine.h"
//Should work for receiving SET, UA and DISC

unsigned char * receive_tram(int fd)
{
    // the first parameter of calloc is 3 because right now the function only works for non info trams and the flags aren't included
    unsigned char * result = calloc(3, sizeof(unsigned char));
    enum reception_state state = start;

    unsigned char currentByte = 0x00;
    int res, continue_loop = 1;
    
    while(continue_loop)
    {
        res = read(fd, &currentByte, 1);
        if (res != 1) fprintf(stderr, "Failed to read in receive_tram!\n");

        switch(state)
        {
            case start:
            {
                if (currentByte == FLAG) state = flag_rcv;
                break;
            }
            case flag_rcv:
            {
                if (currentByte == COMM_SEND_REP_REC || currentByte == COMM_REC_REP_SEND)
                {
                    state = a_rcv;
                    result[0] = currentByte;
                }
                else if (currentByte != FLAG) state = start;
                break;
            }
            case a_rcv:
            {
                if (currentByte == UA || currentByte == DISC || currentByte == SET || currentByte == REJ || currentByte == (REJ | R_MASK) || currentByte == RR || currentByte == (RR|R_MASK))
                {
                    state = c_rcv;
                    if (currentByte == (RR | R_MASK)) result[1] == RR;
                    else if (currentByte == (REJ | R_MASK)) result[1] == REJ;
                    else result[1] = currentByte;
                }
                else if (currentByte == FLAG) state = flag_rcv;
                else state = start;
                break;
            }
            case c_rcv:
            {
                if (currentByte == (result[0] ^ result[1]))
                {
                    state = bcc_ok;
                    result[2] = currentByte;
                }
                else if (currentByte == FLAG) state = flag_rcv;
                else state = start;
                break;
            }
            case bcc_ok:
            {
                if (currentByte == FLAG) state = stop;
                else state = start;
                break;
            }
            case stop:
            {
                continue_loop = 0;
                break;
            }
            default: fprintf(stderr, "Invalid reception state!\n");
        }
    }
    
    return result;
}

unsigned char * receive_info_tram(int fd, int *data_size)
{
    unsigned char * result = calloc(255, sizeof(unsigned char));
    enum reception_info_state state = start;

    unsigned char currentByte = 0x00;
    int res, continue_loop = 1, currentIndex = 0;
    
    while(continue_loop)
    {
        res = read(fd, &currentByte, 1);
        if (res != 1) fprintf(stderr, "Failed to read in receive_tram!\n");

        switch(state)
        {
            case start:
            {
                if (currentByte == FLAG) state = flag_rcv;
                break;
            }
            case flag_rcv:
            {
                if (currentByte == COMM_SEND_REP_REC || currentByte == COMM_REC_REP_SEND)
                {
                    state = a_rcv;
                    result[0] = currentByte;
                }
                else if (currentByte != FLAG) state = start;
                break;
            }
            case a_rcv:
            {
                if (currentByte == UA || currentByte == DISC || currentByte == SET || currentByte == REJ || currentByte == (REJ | R_MASK) || currentByte == RR || currentByte == (RR|R_MASK))
                {
                    state = c_rcv;
                    if (currentByte == (RR | R_MASK)) result[1] == RR;
                    else if (currentByte == (REJ | R_MASK)) result[1] == REJ;
                    else result[1] = currentByte;
                }
                else if (currentByte == FLAG) state = flag_rcv;
                else state = start;
                break;
            }
            case c_rcv:
            {
                if (currentByte == (result[0] ^ result[1]))
                {
                    state = receiving_data;
                    result[2] = currentByte;
                    currentIndex = 3;
                }
                else if (currentByte == FLAG) state = flag_rcv;
                else state = start;
                break;
            }
            case receiving_data:
            {
                if (currentByte == FLAG) state = stop;
                else
                {
                    result[currentIndex++] = currentIndex;
                    continue;
                } 
                break;
            }
            case stop:
            {
                continue_loop = 0;
                break;
            }
            default: fprintf(stderr, "Invalid reception state!\n");
        }
    }
    (*data_size) = currentIndex + 1;
    return result;
}
