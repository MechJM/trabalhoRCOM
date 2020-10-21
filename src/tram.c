#include "tram.h"

void setup_rs()
{
    r = 1;
    s = 0;
    last_s = -1;
    last_r = -1;
}

unsigned char *generate_info_tram(unsigned char *data, unsigned char address, int array_size)
{
    unsigned char *tram = malloc((6 + array_size) * sizeof(unsigned char));

    tram[0] = FLAG;
    tram[1] = address;

    unsigned char actual_control = INFO_CTRL;

    if (s > 0)
    {
        actual_control |= S_MASK;
        s--;
    }
    else
        s++;

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

void parse_and_process_su_tram(unsigned char * tram, int fd)
{
    unsigned char * response;
    int res;

    switch (tram[1])
    {
        case SET:
        {
            printf("Received request to start communication. Acknowledging.\n");
            response = generate_su_tram(COMM_SEND_REP_REC, UA);
            break;
        }
        case UA:
        {
            if ()
            printf("Received request to start communication. Acknowledging.\n");
            response = generate_su_tram(COMM_SEND_REP_REC, UA);
            break;
        }
        default: printf()
    }

    res = write(fd,response, NON_INFO_TRAM_SIZE);
    printf("%d Bytes Written\n", res);
}

struct parse_results * parse_tram(unsigned char *tram, int tram_size)
{
    //Tram must be unstuffed before being passed to this function, flags should not be included in the tram passed
    
    struct parse_results * result = calloc(1, sizeof(struct parse_results));
    //Setting default values
    result->received_data = NULL;
    result->tram_size = tram_size;
    result->duplicate = 0;
    result->data_integrity = 1;
    result->header_validity = 1;
    result->control_field = 0;
    result->address_field = 0;

    unsigned char * data_parsed = calloc(tram_size - 4, sizeof(unsigned char));
    /*
    if ((tram[0] != COMM_SEND_REP_REC && tram[0] != COMM_REC_REP_SEND)                                                                                                                                                  //Checks if the second byte matches one of the possible values for the address field
        || (tram[1] != INFO_CTRL && tram[1] != (INFO_CTRL | S_MASK) && tram[1] != SET && tram[1] != DISC && tram[1] != UA && tram[1] != RR && tram[1] != (RR | R_MASK) && tram[1] != REJ && tram[1] != (REJ | R_MASK))) //Checks if the third byte matches one of the possible values for the control field
        result->header_validity = 0;

    unsigned char bcc1 = tram[0] ^ tram[1];

    if (bcc1 != tram[2]) result->header_validity = 0;*/

    int is_info_tram = 0;

    switch (tram[1])
    {
        case SET:
        {
            printf("Request to start communication received. Acknowledging.\n"); 
            break;
        }
        case UA:
        {
            printf("Request to start/end communication was acknowledged.\n");
            break;
        }
        case INFO_CTRL:
        {
            if (last_s == -1) last_s = 0;
            else if (last_s == 0) result->duplicate = 1;
            else last_s = 0;
            is_info_tram = 1;
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
            is_info_tram = 1;
            for (int i = 3; i < (tram_size + 3 - 4); i++)
            {
                data_parsed[i - 3] = tram[i];
            }
            printf("Data tram received.\n");
            break;
        }
        case DISC:
        {
            printf("Connection ended.\n");
            break;
        }
        case RR:
        {
            if (last_r == -1) last_r = 0;
            else if(last_r == 0) result->duplicate = 1;
            else last_r = 0;
            printf("Data was sent without issues. Positive acknowledgment.\n");
            break;
        }
        case (RR | R_MASK):
        {
            if (last_r == -1) last_r = 1;
            else if(last_r == 1) result->duplicate = 1;
            else last_r = 1;
            printf("Data was sent without issues. Positive acknowledgment.\n");
            break;
        }
        case REJ:
        {
            if (last_r == -1) last_r = 0;
            else if(last_r == 0) result->duplicate = 1;
            else last_r = 0;
            printf("Data sent had issues. Negative acknowledgment.\n");
            break;
        }
        case (REJ | R_MASK):
        {
            if (last_r == -1) last_r = 1;
            else if(last_r == 1) result->duplicate = 1;
            else last_r = 1;
            printf("Data sent had issues. Negative acknowledgment.\n");
            break;
        }
        default: result->header_validity = 0;
    }

    result->address_field = tram[0];
    result->control_field = tram[1];
    result->received_data = data_parsed;

    unsigned char bcc2 = 0x00;

    if (is_info_tram)
    {
        for (int i = 3; i < (tram_size - 1); i++)
        {
            bcc2 ^= tram[i];
        }
    }

    if (bcc2 != tram[tram_size - 1]) result->data_integrity = 0;

    return result;
}

void process_tram_received(struct parse_results * results, int port)
{
    unsigned char *response;
    int response_size = 0;

    if (!results->header_validity) return;
    if (results->received_data != NULL && results->header_validity && results->data_integrity)
    {
        if (!results->duplicate)
        {
            for (int i = 0; i < results->tram_size - 4; i++)
            {
                //packet[data_bytes_received++] = results->received_data[i];
            }
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
                response = generate_info_tram(data_to_be_sent, COMM_SEND_REP_REC, to_be_sent_size);
                response_size = to_be_sent_size + 6;
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
            last_data_sent = data_to_be_sent;
            response = generate_info_tram(data_to_be_sent, COMM_SEND_REP_REC, to_be_sent_size);
            response_size = to_be_sent_size + 6;
            break;
        }
        case RR | R_MASK:
        {
            last_data_sent = data_to_be_sent;
            response = generate_info_tram(data_to_be_sent, COMM_SEND_REP_REC, to_be_sent_size);
            response_size = to_be_sent_size + 6;
            break;
        }
        case REJ:
        {
            response = generate_info_tram(last_data_sent, COMM_SEND_REP_REC, last_data_size);
            response_size = last_data_size + 6;
            break;
        }
        case REJ | R_MASK:
        {
            response = generate_info_tram(last_data_sent, COMM_SEND_REP_REC, last_data_size);
            response_size = last_data_size + 6;
            break;
        }
        default: break;
            //fprintf(stderr, "Invalid control field! Value: %d\n", results->control_field);
    }

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