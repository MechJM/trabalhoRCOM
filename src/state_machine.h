#include "tram.h"

#pragma once

int timeout;

enum reception_state
{
    start,
    flag_rcv,
    a_rcv,
    c_rcv,
    bcc_ok,
    stop
};

enum reception_info_state
{
    start,
    flag_rcv,
    a_rcv,
    c_rcv,
    receiving_data,
    stop  
};

unsigned char * receive_tram(int control, int fd);

unsigned char * receive_info_tram(int fd, int *data_size);
