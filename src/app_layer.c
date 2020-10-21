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

void restoreFile(char *fileName, unsigned char *packet[], int packet_num)
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
    packet = calloc(packet_num, packet_size);
    printf("Created %d Packets...\n", packet_num);
    savePackets(packet, fileData);
    printf("Packets Ready To Be Sent!\n");
    /*
    char *restoredFileName = "restoreFile.gif";
    restoreFile(restoredFileName, packet, packet_num);
    */
}