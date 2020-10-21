#include "tram.h"

void setup_initial_values()
{
    r = 1;
    s = 0;
    last_s = -1;
    last_r = -1;
    last_seq = -1;
}

unsigned char *generate_info_tram(unsigned char *data, unsigned char address, int array_size)
{
    unsigned char *tram = malloc((6 + array_size) * sizeof(unsigned char));

    tram[0] = FLAG;
    tram[1] = address;

    unsigned char actual_control = INFO_CTRL;

    if (last_seq == -1 || last_seq == 1) last_seq = 0;
    else if (last_seq == 0) last_seq = 1;

    if (last_seq == 1) actual_control |= S_MASK;
    /*
    if (s > 0)
    {
        actual_control |= S_MASK;
        s--;
    }
    else
        s++;
    */
    tram[2] = actual_control;
    tram[3] = address ^ actual_control;

    unsigned char bcc2 = 0x00;

    for (int i = 4; i < (array_size + 4); i++)
    {
        tram[i] = data[i - 4];
        bcc2 ^= tram[i];
    }

    tram[4 + array_size] = bcc2;
    tram[5 + array_size] = FLAG;


    //Storing the last info tram sent
    last_tram_sent = tram;
    last_tram_sent_size = array_size + 6;

    return tram;
}

unsigned char *generate_su_tram(unsigned char address, unsigned char control)
{
    unsigned char *tram = malloc(5 * sizeof(unsigned char));
    tram[0] = FLAG;
    tram[1] = address;

    unsigned char actual_control = control;

    if (control == RR || control == REJ)
    {
        if (r > 0)
        {
            actual_control |= R_MASK;
            r--;
        }
        else
            r++;
    }

    tram[2] = actual_control;
    tram[3] = address ^ actual_control;
    tram[4] = FLAG;

    return tram;
}

int parse_and_process_su_tram(unsigned char * tram, int fd)
{
    unsigned char * response;
    int res;
    int result = DO_NOTHING; //boolean that indicates if we can start to send data

    switch (tram[1])
    {
        case SET:
        {
            printf("Received request to start communication Acknowledging.\n");
            response = generate_su_tram(COMM_SEND_REP_REC, UA);
            break;
        }
        case UA:
        {
            if (sender)
            {
                printf("Communication start request was acknowledged. Starting to send data.\n");
                return SEND_NEW_DATA;
            }
            else
            {
                printf("Communication ended.\n");
                return DO_NOTHING;
            } 
            break;
        }
        case DISC:
        {
            if (sender)
            {
                printf("Communication end request was acknowledged. Acknowledging end.\n");
                response = generate_su_tram(COMM_REC_REP_SEND, UA);
            }
            else
            {
                printf("Received request to end communication. Ending communication.\n");
                response = generate_su_tram(COMM_REC_REP_SEND, DISC);
            }
            break;
        }
        case REJ:
        {
            printf("Last info packet sent had issues. Resending.\n");
            return RESEND_DATA;
            break;
        }
        case RR:
        {   
            printf("Last info packet sent had no issues. Proceesing.\n");
            return SEND_NEW_DATA;
            break;
        }
        default: printf("Invalid control byte!\n");
    }
    
    res = write(fd,response, NON_INFO_TRAM_SIZE);
    printf("%d Bytes Written\n", res);
    return result;
}

struct parse_results * parse_info_tram(unsigned char *tram, int tram_size)
{
    //Tram must be unstuffed before being passed to this function, flags should not be included in the tram passed
    
    struct parse_results * result = calloc(1, sizeof(struct parse_results));
    //Setting default values
    result->received_data = NULL;
    result->tram_size = tram_size;
    result->duplicate = 0;
    result->data_integrity = 1;
    result->control_bit = 0;
    result->header_validity = 1;

    unsigned char * data_parsed = calloc(tram_size - 4, sizeof(unsigned char));

    if ((tram[0] != COMM_SEND_REP_REC)                                                                                                                                                  //Checks if the second byte matches one of the possible values for the address field
        || (tram[1] != INFO_CTRL && tram[1] != (INFO_CTRL | S_MASK))) //Checks if the third byte matches one of the possible values for the control field
        result->header_validity = 0;

    unsigned char bcc1 = tram[0] ^ tram[1];

    if (bcc1 != tram[2]) result->header_validity = 0;

    switch (tram[1])
    {
        case INFO_CTRL:
        {
            //Refactor use of last_s and last_r
            if (last_s == -1) last_s = 0;
            else if (last_s == 0) result->duplicate = 1;
            else last_s = 0;
            for (int i = 3; i < (tram_size + 3 - 4); i++)
            {
                data_parsed[i - 3] = tram[i];
            }
            printf("Data tram received.\n");
            break;
        }
        case (INFO_CTRL | S_MASK):
        {
            if (last_s == -1) last_s = 1;
            else if(last_s == 1) result->duplicate = 1;
            else last_s = 1;
            for (int i = 3; i < (tram_size + 3 - 4); i++)
            {
                data_parsed[i - 3] = tram[i];
            }
            printf("Data tram received.\n");
            break;
        }
        default: result->header_validity = 0;
    }

    result->received_data = data_parsed;

    unsigned char bcc2 = 0x00;

    
    for (int i = 3; i < (tram_size - 1); i++)
    {
        bcc2 ^= tram[i];
    }
    

    if (bcc2 != tram[tram_size - 1]) result->data_integrity = 0;

    return result;
}

void process_info_tram_received(struct parse_results * results, int port)
{
    unsigned char *response;
    int response_size = 0;

    if (!results->header_validity) return;
    if (results->received_data != NULL && results->header_validity && results->data_integrity)
    {
        if (!results->duplicate)
        {
            packet[data_trams_received++] = results->received_data; //May or may not work
        }
        else free(results->received_data);
        response  = generate_su_tram(COMM_SEND_REP_REC,RR);
        response_size = 5;
    }

    if (results->received_data != NULL && results->header_validity && !results->data_integrity)
    {
        if (!results->duplicate) response = generate_su_tram(COMM_SEND_REP_REC,REJ);
        else response = generate_su_tram(COMM_SEND_REP_REC,RR);
        response_size = 5;
    }

    /*
    switch (results->control_field)
    {
        case SET:
        {
            response = generate_su_tram(COMM_SEND_REP_REC, UA);
            response_size = 5;
            break;
        }
        case UA:
        {
            if (sender)
            {
                response = generate_info_tram(packet[last_packet_index++], COMM_SEND_REP_REC, packet_size);
                response_size = packet_size + 6;
            }
            else return;
            break;
        }
        case DISC:
        {
            if (!sender) response = generate_su_tram(COMM_REC_REP_SEND, DISC);
            else response = generate_su_tram(COMM_REC_REP_SEND, UA);
            response_size = 5;
            break;
        }
        case RR:
        {
            if (last_packet_index == packet_num) return;
            response = generate_info_tram(packet[last_packet_index++], COMM_SEND_REP_REC, packet_size);
            response_size = packet_size + 6;
            break;
        }
        case RR | R_MASK:
        {
            if (last_packet_index == packet_num) return;
            response = generate_info_tram(packet[last_packet_index++], COMM_SEND_REP_REC, packet_size);
            response_size = packet_size + 6;
            break;
        }
        case REJ:
        {
            response = last_tram_sent;
            response_size = last_tram_sent_size;
            break;
        }
        case REJ | R_MASK:
        {
            response = last_tram_sent;
            response_size = last_tram_sent_size;
            break;
        }
        default: break;
            //fprintf(stderr, "Invalid control field! Value: %d\n", results->control_field);
    }*/

    //TODO Find better way to figure out which data needs to be/was sent

    int res = write(port, response, response_size);
    printf("%d Bytes Written\n", res);
}

unsigned char * translate_array(unsigned char * array, int offset, int array_size, int starting_point)
{
    unsigned char * new_array = calloc(array_size + offset, sizeof(unsigned char));

    for (int i = 0; i < (array_size + offset); i++)
    {
        if (i < starting_point)
        {
            new_array[i] = array[i];
        }
        else if (offset > 0)
        {
            new_array[i + offset] = array[i];
        }
        else
        {
            new_array[i] = array[i - offset];
        }
    }
    
    free(array);
    return new_array;
}

void byte_stuff(unsigned char * tram, int * tram_size)
{
    for (int i = 4; i < ((*tram_size) - 1); i++)
    {
        if (tram[i] == FLAG)
        {
            tram = translate_array(tram,1,(*tram_size),i);
            (*tram_size)++;
            tram[i] = ESC_BYTE_1;
            tram[++i] = ESC_BYTE_2;
        }
        else if (tram[i] == ESC_BYTE_1)
        {
            tram = translate_array(tram,1,(*tram_size),i);
            (*tram_size)++;
            tram[i] = ESC_BYTE_1;
            tram[++i] = ESC_BYTE_3;
        }
    }
}

void byte_unstuff(unsigned char * tram, int * tram_size)
{
    for (int i = 3; i < (*tram_size); i++)
    {
        if (tram[i] == ESC_BYTE_1 && tram[i + 1] == ESC_BYTE_2)
        {
            tram = translate_array(tram,-1,(*tram_size),i);
            (*tram_size)--;
            tram[i] = FLAG;
        }
        else if (tram[i] == ESC_BYTE_1 && tram[i + 1] == ESC_BYTE_3)
        {
            tram = translate_array(tram,-1,(*tram_size),i);
            (*tram_size)--;
            tram[i] = ESC_BYTE_1;
        }
    }
}