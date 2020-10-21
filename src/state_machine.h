#include "tram.h"

#pragma once

int timeout;

enum set_states
{
    start,
    flag_rcv,
    a_rcv,
    c_rcv,
    bcc_ok,
    stop
};

unsigned char * receive_tram(int control, int fd);

void receive_info_tram(int fd);
