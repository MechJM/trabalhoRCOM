#include "state_machine.h"
//Should work for receiving SET, UA and DISC

unsigned char *receive_tram(int fd)
{
    // the first parameter of calloc is 3 because right now the function only works for non info trams and the flags aren't included
    unsigned char *result = calloc(3, sizeof(unsigned char));
    enum reception_state state = start;

    unsigned char currentByte = 0x00;
    int res, continue_loop = 1;

    while (continue_loop)
    {
        res = read(fd, &currentByte, 1);
        if (res != 1)
            fprintf(stderr, "Failed to read in receive_tram!\n");

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
                if (currentByte == (RR | R_MASK))
                    result[1] = RR;
                else if (currentByte == (REJ | R_MASK))
                    result[1] = REJ;
                else
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
    }
    /*
    printf("First byte: %d\n",result[0]);
    printf("Second byte: %d\n",result[1]);
    printf("Third byte: %d\n",result[2]);
    */
    return result;
}

unsigned char *receive_info_tram(int fd, int *data_size)
{
    unsigned char *result = calloc(255, sizeof(unsigned char));
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