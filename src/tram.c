#include "tram.h"

void setup_rs()
{
    r = 1;
    s = 0;
}

char * generate_info_tram(char * data,unsigned char address,int array_size)
{
    char * tram = malloc((6 + array_size) * sizeof(unsigned char));
    
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

char * generate_su_tram(unsigned char address, unsigned char control)
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