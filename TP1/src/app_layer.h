#pragma once

//Control field values
#define DATA 1
#define START 2
#define END 3

//Type field values
#define FILE_SIZE 0
#define FILE_NAME 1

#include "tram.h"

unsigned char *readFile(unsigned char *fileName);

unsigned char *splitFileData(unsigned char *fileData, int x, int packet_size);

void savePackets(unsigned char *packet[], unsigned char *fileData);

void restoreFile(char *fileName, unsigned char *packet[], int packet_size, int packet_num, long int file_size);

void restoreSimpleFile(char *fileName, unsigned char *fileData, long int file_size);

void deleteFile(char *fileName);

void processFile(unsigned char *fileData);

unsigned char * generate_data_packet(int seq_num, int byte_num,const unsigned char * data);

unsigned char * generate_control_packet(unsigned char control_field, int param_num, int * t_values, int * l_values, unsigned char ** values);

void extract_size_name(unsigned char *tram, unsigned char *size, unsigned char *name);

void extract_seq_size_data(unsigned char * tram, int * seq, int * size, unsigned char * data);