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
    tram[3] = (FLAG ^ address) ^ actual_control;

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
    tram[3] = (FLAG ^ address) ^ actual_control;
    tram[4] = FLAG;

    return tram;
}

int parse_tram(char * tram, int tram_size, char * data_parsed)
{
    if ((tram[0] != FLAG) // Checks if the first byte matches FLAG
    || (tram[1] != COMM_SEND_REP_REC && tram[1] != COMM_REC_REP_SEND) //Checks if the second byte matches one of the possible values for the address field 
    || (tram[2] != INFO_CTRL && tram[2] != (INFO_CTRL | S_MASK) && tram[2] != SET && tram[2] != DISC && tram[2] != UA && tram[2] != RR && tram[2] != (RR | R_MASK) && tram[2] != REJ && tram[2] != (REJ | R_MASK))) //Checks if the third byte matches one of the possible values for the control field
    return WRONG_HEADER;

    //TODO 
}