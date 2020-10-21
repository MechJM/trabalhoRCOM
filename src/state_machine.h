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
    start_info,
    flag_rcv_info,
    a_rcv_info,
    c_rcv_info,
    receiving_data_info,
    stop_info  
};

unsigned char * receive_tram(int fd);

unsigned char * receive_info_tram(int fd, int *data_size);
