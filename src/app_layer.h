#pragma once

//Control field values
#define DATA 1
#define START 2
#define END 3

#include "tram.h"

unsigned char *readFile(unsigned char *fileName);

unsigned char *splitFileData(unsigned char *fileData, int x, int packet_size);

void savePackets(unsigned char *packet[], unsigned char *fileData);

void restoreFile(char *fileName, unsigned char *packet[], int packet_num);

void restoreSimpleFile(char *fileName, unsigned char *fileData, long int file_size);

void processFile(unsigned char *fileData);

unsigned char * generate_data_packet(int seq_num, int byte_num, unsigned char * data);

unsigned char * generate_control_packet(unsigned char control_field, int param_num, int * t_values, int * l_values, unsigned char ** values);