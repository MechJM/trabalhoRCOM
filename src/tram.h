#pragma once

#define INFO_CTRL 0x00
#define FLAG      0x7e
#define SET       0x03
#define DISC      0x0b
#define UA        0x07
#define RR        0x05
#define REJ       0x01
#define R_MASK    0x80
#define S_MASK    0x40

#define COMM_SEND_REP_REC 0x03
#define COMM_REC_REP_SEND 0x01

int r,s;

void setup_rs();

char * generate_info_tram(char * data,unsigned char address,int array_size);

char * generate_su_tram(unsigned char address, unsigned char control);