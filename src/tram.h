#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#pragma once

// Flag
#define FLAG 0x7e

// Control Field
#define INFO_CTRL 0x00
#define SET 0x03
#define DISC 0x0b
#define UA 0x07
#define RR 0x05
#define REJ 0x01
#define R_MASK 0x80
#define S_MASK 0x40

// Address Field
#define COMM_SEND_REP_REC 0x03
#define COMM_REC_REP_SEND 0x01

//Escape sequences
#define ESC_BYTE_1 0x7d
#define ESC_BYTE_2 0x5e
#define ESC_BYTE_3 0x5d

//Data received
unsigned char ** packet;

//Parse results
struct parse_results
{
    unsigned char * received_data; //NULL if it's not info tram
    int tram_size; //size of the tram in bytes
    int duplicate; //boolean to indicate if the tram received is a duplicate or not
    int data_integrity; //boolean to indicate if bcc2 checks out
    int header_validity; //boolean to indicate if bbc1 checks out and the values are valid
    unsigned char control_field; //value of the control field
    unsigned char address_field; //value of the address field
};

int r, s;

int last_s,last_r;

long int data_bytes_received;

int sender; // boolean that indicates whether the program running is the sender or the receiver 

unsigned char * last_data_sent, *data_to_be_sent;

int last_data_size, to_be_sent_size;

void setup_rs();

unsigned char *generate_info_tram(unsigned char *data, unsigned char address, int array_size);

unsigned char *generate_su_tram(unsigned char address, unsigned char control);

struct parse_results * parse_tram(unsigned char *tram, int tram_size);

void process_tram_received(struct parse_results * results, int port);

unsigned char * translate_array(unsigned char * array, int offset, int array_size, int starting_point);

void byte_stuff(unsigned char * tram, int * tram_size);

void byte_unstuff(unsigned char * tram, int * tram_size);