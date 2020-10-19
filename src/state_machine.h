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

unsigned char * receive_tram(int control, int fd);