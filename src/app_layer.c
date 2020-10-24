/* APP LAYER FILE HANDLING */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "app_layer.h"

long int file_size;
int packet_size = 127;
int packet_num;

unsigned char *readFile(unsigned char *fileName)
{
    printf("Reading File <%s>\n", fileName);
    FILE *f;
    struct stat metadata;
    unsigned char *fileData;
    if ((f = fopen((char *)fileName, "rb")) == NULL)
    {
        perror("Error Read File!");
    }
    stat((char *)fileName, &metadata);
    file_size = metadata.st_size;
    printf("File Size =  %ld Bytes\n", file_size);
    fileData = (unsigned char *)malloc(file_size);
    fread(fileData, sizeof(unsigned char), file_size, f);
    fclose(f);
    return fileData;
}

unsigned char *splitFileData(unsigned char *fileData, int x, int packet_size)
{
    unsigned char *packet_temp = (unsigned char *)malloc(packet_size);
    int i;
    int j;
    for (j = 0, i = x; i < packet_size; i++, j++)
    {
        packet_temp[j] = fileData[i];
    }
    return packet_temp;
}

void savePackets(unsigned char *packet[], unsigned char *fileData)
{
    for (int i = 0; i < packet_num; i++)
    {
        packet[i] = splitFileData(fileData, packet_size * i, i * packet_size + packet_size);
        printf("Packet[%d] Saved!\n", i);
    }
}

void restoreFile(char *fileName, unsigned char *packet[], int packet_size, int packet_num)
{
    printf("Restoring File...\n");
    FILE *f = fopen((char *)fileName, "wb+");
    for (int i = 0; i < packet_num; i++)
    {
        fwrite((void *)packet[i], 1, packet_size, f);
    }
    printf("File Restored!\n");
    fclose(f);
}

void restoreSimpleFile(char *fileName, unsigned char *fileData, long int file_size)
{
    printf("Restoring Simple File...\n");
    FILE *f = fopen((char *)fileName, "wb+");
    fwrite((void *)fileData, 1, file_size, f);
    printf("New File Created!\n");
    fclose(f);
}

void processFile(unsigned char *fileData)
{
    if (file_size % packet_size != 0)
    {
        packet_num = file_size / packet_size + 1;
    }
    else
    {
        packet_num = file_size / packet_size;
    }
    //packet = calloc(packet_num, packet_size);
    packet = malloc(sizeof(unsigned char *));
    *packet = calloc(255,sizeof(unsigned char));
    printf("Created %d Packets...\n", packet_num);
    savePackets(packet, fileData);
    printf("Packets Ready To Be Sent!\n");
    /*
    char *restoredFileName = "restoreFile.gif";
    restoreFile(restoredFileName, packet, packet_num);
    */
}

unsigned char * generate_data_packet(int seq_num, int byte_num, unsigned char * data)
{
    unsigned char * result = calloc(byte_num + 4, sizeof(unsigned char));

    result[0] = DATA;
    result[1]  = seq_num;

    int l2 = (int) byte_num / 256;
    int l1 = byte_num % 256;

    result[2] = l2;
    result[3] = l1;
    
    for (int i = 4; i < (byte_num + 4); i++)
    {
        result[i] = data[i - 4];
    }
    
    return result;
}

unsigned char * generate_control_packet(unsigned char control_field, int param_num, int * t_values, int * l_values, unsigned char ** values)
{
    int total_byte_num = 1 + 2 * param_num;
    for (int i = 0; i < param_num; i++)
    {
        total_byte_num += l_values[i];
    }

    unsigned char * result = calloc(total_byte_num, sizeof(unsigned char));

    result[0] = control_field;

    int nextIndex = 1;

    for (int i = 0; i < param_num; i++)
    {
        result[nextIndex++] = t_values[i];
        result[nextIndex++] = l_values[i];
        int currentDataSize = l_values[i];
        for (int j = 0; j < currentDataSize; j++)
        {
            result[nextIndex++] = values[i][j];
        }
        
    }
    
    return result;
}

void extract_size_name(unsigned char * tram, unsigned char * size, unsigned char * name)
{
    int middle_of_data = 0, extracted_name = 0, extracted_size = 0;
    int i = 1;
    int data_index = 0;
    unsigned char type, length;

    while(!extracted_size || !extracted_name)
    {
        if (!middle_of_data)
        {
            type = tram[i];
            length = tram[++i];
            middle_of_data = 1;
            data_index = 0;
        }
        else
        {   
           if (type == FILE_SIZE)
           {
               if (length == 0)
               {
                   extracted_size = 1;
                   middle_of_data = 0;
                   continue;
               }
               size[data_index++] = tram[i];
           }
           else if (type == FILE_NAME)
           {
               if (length == 0)
               {
                   extracted_name = 1;
                   middle_of_data = 0;
                   continue;
               }
               name[data_index++] = tram[i];
           }
           length--;
        }
        i++;
    }
}