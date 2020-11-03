#include "state_machine.h"
/*
unsigned char * receive_tram(int fd)
{
    unsigned char * first_tram_part = calloc(4, sizeof(unsigned char));
    unsigned char * result;
    int res;

    res = read(fd, first_tram_part, 4);
    if (res != 4) fprintf(stderr, "Failed to read initial packet part in receive_tram function!\n");
    
    if ((first_tram_part[0] != FLAG)
    || (first_tram_part[1] != COMM_REC_REP_SEND && first_tram_part[1] != COMM_SEND_REP_REC) 
    || (first_tram_part[2] != SET && first_tram_part[2] != DISC && first_tram_part[2] != UA && first_tram_part[2] != RR && first_tram_part[2] != (RR | R_MASK) && first_tram_part[2] != REJ && first_tram_part[2] != (REJ | R_MASK) && first_tram_part[2] != INFO_CTRL && first_tram_part[2] != (INFO_CTRL | S_MASK))
    || (first_tram_part[3] != (first_tram_part[1] ^ first_tram_part[2]))
    ) 
    result = NULL;

    unsigned char current_byte = 0x00;

    if (first_tram_part[2] == INFO_CTRL || first_tram_part[2] == (INFO_CTRL | S_MASK))
    {
        result = calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
        int current_index = 3;
        for (int i = 0; i < 3; i++)
        {
            result[i] = first_tram_part[i + 1];
        }
        while (!reached_timeout)
        {
            res = read(fd, &current_byte, 1);
            if (res != 1) fprintf(stderr, "Failed to read while trying to receive info tram in receive_tram function!\n");
            if (current_byte == FLAG) break;
            else result[current_index++] = current_byte;
        }
    }
    else
    {
        result = calloc(3, sizeof(unsigned char));
        for (int i = 0; i < 3; i++)
        {
            result[i] = first_tram_part[i + 1];
        }
        res = read(fd, &current_byte, 1);
        if (res != 1) fprintf(stderr, "Failed to read while trying to receive terminating FLAG of S/U tram in receive_tram function!\n");
        if (current_byte != FLAG) result = NULL;
    }

    free(first_tram_part);

    if (reached_timeout) result = NULL;

    return result;
}
*/
//Should work for receiving SET, UA and DISC
unsigned char *receive_tram(int fd)
{
    // the first parameter of calloc is 3 because this function only works for info trams and the flags aren't included
    unsigned char *result = calloc(3, sizeof(unsigned char));
    enum reception_state state = start;

    unsigned char currentByte = 0x00;
    int res, continue_loop = 1;
    //printf("Byte\tState\n");
    while (continue_loop && !reached_timeout)
    {
        res = read(fd, &currentByte, 1);
        if (res != 1) fprintf(stderr, "Failed to read in receive_tram!\n");
        //printf("%x\t",currentByte);
        switch (state)
        {
            case start:
            {
                if (currentByte == FLAG)
                    state = flag_rcv;
                break;
            }
            case flag_rcv:
            {
                if (currentByte == COMM_SEND_REP_REC || currentByte == COMM_REC_REP_SEND)
                {
                    state = a_rcv;
                    result[0] = currentByte;
                }
                else if (currentByte != FLAG)
                    state = start;
                break;
            }
            case a_rcv:
            {
                if (currentByte == UA || currentByte == DISC || currentByte == SET || currentByte == REJ || currentByte == (REJ | R_MASK) || currentByte == RR || currentByte == (RR | R_MASK))
                {
                    state = c_rcv;
                    result[1] = currentByte;
                }
                else if (currentByte == FLAG)
                    state = flag_rcv;
                else
                    state = start;
                break;
            }
            case c_rcv:
            {
                if (currentByte == (result[0] ^ result[1]))
                {
                    state = bcc_ok;
                    result[2] = currentByte;
                }
                else if (currentByte == FLAG)
                    state = flag_rcv;
                else
                    state = start;
                break;
            }
            case bcc_ok:
            {
                if (currentByte == FLAG)
                {
                    continue_loop = 0;
                }
                else
                    state = start;
                break;
            }
            default:
            {
                fprintf(stderr, "Invalid reception state!\n");
                break;
            }
        }
        //printf("%x\n",state);
    }

    if (reached_timeout)
    {
        return NULL;
    } 
    
    return result;
}

unsigned char *receive_info_tram(int fd, int *data_size)
{
    unsigned char *result = calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
    enum reception_info_state state = start_info;

    unsigned char currentByte = 0x00;
    int res, continue_loop = 1, currentIndex = 0;
    //printf("Byte\tState\n");
    while (continue_loop)
    {
        //printf("Cheguei aqui\n");
        res = read(fd, &currentByte, 1);
        
        //printf("%x\t",currentByte);
        if (res != 1)
            fprintf(stderr, "Failed to read in receive_tram!\n");
        //if (currentByte == 0x51) printf("State: %d\n",state);

        switch (state)
        {
            case start_info:
            {
                if (currentByte == FLAG)
                    state = flag_rcv_info;
                break;
            }
            case flag_rcv_info:
            {
                if (currentByte == COMM_SEND_REP_REC || currentByte == COMM_REC_REP_SEND)
                {
                    state = a_rcv_info;
                    result[0] = currentByte;
                }
                else if (currentByte != FLAG)
                    state = start_info;
                break;
            }
            case a_rcv_info:
            {
                if (currentByte == INFO_CTRL || currentByte == (INFO_CTRL | S_MASK))
                {
                    state = c_rcv_info;
                    result[1] = currentByte;
                }
                else if (currentByte == FLAG)
                    state = flag_rcv_info;
                else
                    state = start_info;
                break;
            }
            case c_rcv_info:
            {
                if (currentByte == (result[0] ^ result[1]))
                {
                    state = receiving_data_info;
                    result[2] = currentByte;
                    currentIndex = 3;
                }
                else if (currentByte == FLAG)
                    state = flag_rcv_info;
                else
                    state = start_info;
                break;
            }
            case receiving_data_info:
            {
                if (currentByte == FLAG)
                {
                    continue_loop = 0;
                }
                    
                else
                {
                    result[currentIndex++] = currentByte;
                    continue;
                }
                break;
            }
            default:
            {
                fprintf(stderr, "Invalid reception state!\n");
                break;
            }  
        }
        //printf("%d\n",state);
    }
    (*data_size) = currentIndex;
    return result;
}