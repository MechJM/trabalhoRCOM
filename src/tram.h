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

// Parse Output Values
#define WRONG_HEADER 1
#define START_COMMUNICATION 2
#define ACKNOWLEDGE_START 3
#define DATA_RECEIVED 4
#define END_COMMUNICATION 5
#define NO_ISSUE_DATA 6
#define ISSUE_DATA 7
#define INTEGRITY_FAILED 8

int r, s;

void setup_rs();

unsigned char *generate_info_tram(unsigned char *data, unsigned char address, int array_size);

unsigned char *generate_su_tram(unsigned char address, unsigned char control);

int parse_tram(unsigned char *tram, int tram_size, unsigned char *data_parsed);

void process_tram_received(int parse_result, unsigned char *data_to_be_sent, int data_size, int port);