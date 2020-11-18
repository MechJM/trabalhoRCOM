#include "state_machine.h"
#include <errno.h>
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
/*
unsigned char *receive_info_tram(int fd, int *data_size)
{
    unsigned char *result = calloc(max_array_size, sizeof(unsigned char));
    unsigned char current_byte = 0x00;
    long int res;
    res = read(fd, &current_byte, 1);
    if (res != 1) fprintf(stderr, "Failed to read first flag in receive_info_tram");
    //printf("First byte: %x\n",current_byte);
    //if (current_byte != FLAG) return NULL;

    res = read(fd, result, max_packet_size);
    if (res < 0) fprintf(stderr, "Failed to read in receive_info_tram!\n");

    int i,j, found_flag = 0;
    for (i = 0; i < max_packet_size; i++)
    {
        if (result[i + 1] == FLAG)
        {
            found_flag = 1;
            break;
        } 
    }
    if (found_flag)
    {
        off_t off = lseek(fd,-(max_packet_size - i), SEEK_CUR);
        printf("off:%ld\n",off);
        printf("errno:%d\n",errno);
    }
    else
    {
        for (j = i; j < max_packet_size; j++)
        {
            if (result[j + 1] == FLAG)
            {
                break;
            } 
        }
        off_t off = lseek(fd,-(max_packet_size - j), SEEK_CUR);
        printf("off:%ld\n",off);
        printf("errno:%d\n",errno);
    }
    
    
    

    //printf("res:%ld\n",res);
    int i = 0;

    while (1)
    {
        
        current_byte = result[i];
        //printf("i:%d\n",i);
        if (current_byte == FLAG) break;

        if ((i == 0 && (current_byte != COMM_REC_REP_SEND && current_byte != COMM_SEND_REP_REC))
        || (i == 1 && (current_byte != INFO_CTRL && current_byte != (INFO_CTRL | S_MASK)))
        || (i == 2 && ((result[0] ^ result[1]) != current_byte))) 
            return NULL;
        i++;
    }
    printf("i:%d\n",i);
    (*data_size) = i;

    
    if (found_flag) (*data_size) = i;
    else (*data_size) = j;

    return result;   
}
*/

unsigned char *receive_info_tram(int fd, int *data_size)
{
    unsigned char *result = calloc(max_array_size, sizeof(unsigned char));
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
            fprintf(stderr, "Failed to read in receive_info_tram!\n");
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
