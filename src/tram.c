#include "tram.h"

void setup_rs()
{
    r = 1;
    s = 0;
}

unsigned char * generate_info_tram(char * data,unsigned char address,int array_size)
{
    unsigned char * tram = malloc((6 + array_size) * sizeof(unsigned char));
    
    tram[0] = FLAG;
    tram[1] = address;

    unsigned char actual_control = INFO_CTRL;

    if (s > 0)
    {
        actual_control |= S_MASK;
        s--;
    }
    else s++;

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

unsigned char * generate_su_tram(unsigned char address, unsigned char control)
{
    unsigned char * tram = malloc(5 * sizeof(unsigned char));
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
        else r++;
    }

    tram[2] = actual_control;
    tram[3] = address ^ actual_control;
    tram[4] = FLAG;

    return tram;
}

int parse_tram(unsigned char * tram, int tram_size,unsigned char * data_parsed)
{
    //TEMPORARY
    tram_size=tram_size;
    data_parsed=data_parsed;
    //TEMPORARY
    if (  (tram[0] != COMM_SEND_REP_REC && tram[0] != COMM_REC_REP_SEND) //Checks if the second byte matches one of the possible values for the address field 
    || (tram[1] != INFO_CTRL && tram[1] != (INFO_CTRL | S_MASK) && tram[1] != SET && tram[1] != DISC && tram[1] != UA && tram[1] != RR && tram[1] != (RR | R_MASK) && tram[1] != REJ && tram[1] != (REJ | R_MASK))) //Checks if the third byte matches one of the possible values for the control field
    return WRONG_HEADER;

    switch(tram[1])
    {
        case SET:
        {
            printf("Request to start communication received. Acknowledging.\n");
            return START_COMMUNICATION;
        } 
        case UA:
        {
            printf("Request to start communication was acknowledged.\n");
            return ACKNOWLEDGE_START;
        }
        default: fprintf(stderr,"Invalid control byte! Value: %s\n",&tram[1]);
    }
    //TODO
    return 0;
}

void process_tram_received(int parse_result, unsigned char * data_received, int port)
{
    //TEMPORARY
    data_received=data_received;
    //TEMPORARY

    unsigned char * response;
    int response_size;

    switch(parse_result)
    {
        case START_COMMUNICATION:
        {
            response = generate_su_tram(COMM_SEND_REP_REC,UA);
            response_size = 5;
            break;
        }
        case ACKNOWLEDGE_START:
        {
            //Only for class 2 exercises
            return;
        }
        default: fprintf(stderr,"Invalid parse result! Value: %d\n",parse_result);
    }

    //TEMPORARY
    response_size=response_size;
    response=response;
    port=port;
    //TEMPORARY

    //TODO
    //write(port,response,response_size);
}