/* Non-Canonical Input Processing */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "tram.h"
#include "state_machine.h"
#include "link_layer.h"
#include "app_layer.h"

#define BAUDRATE B38400
#define FALSE 0

volatile int STOP = FALSE;

int main(int argc, char **argv)
{
  
  int fd = 0;
  timeout = 1;
  ll = NULL;
  packet_size = MAX_PACKET_SIZE;

  //Initialize packet
  packet = (unsigned char **) calloc(MAX_ARRAY_SIZE, sizeof(unsigned char *));
  for (int i = 0; i < MAX_ARRAY_SIZE; i++)
  {
      packet[i] = (unsigned char *) calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  }

  if ((argc < 2) ||
      ((strcmp("/dev/ttyS10", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  fd = llopen(0, RECEIVER);

  // First Control Packet
  unsigned char *control_packet_received = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  llread(fd, (char *)control_packet_received);

  unsigned char *size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *name = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  extract_size_name(control_packet_received, size, name);
  long received_size = *((long *)size);

  // Read File Packets Based On Received_Size
  int packet_num;
  if (received_size % packet_size != 0)
    packet_num = received_size / packet_size + 1;
  else
    packet_num = received_size / packet_size;

  unsigned char *tram;
  for (int i = 0; i < packet_num; i++)
  {
    tram = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
    llread(fd, (char *)tram);
    int seq;
    int stored_packet_size;
    extract_seq_size_data(tram, &seq, &stored_packet_size, packet[i]);
  }

  // Last Control Packet
  unsigned char *last_control_packet_received = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  unsigned char *last_size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *last_name = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  llread(fd, (char *)last_control_packet_received);
  extract_size_name(last_control_packet_received, size, name);
  
  if (size == last_size && name == last_name && last_control_packet_received[0] == END)
  {
      printf("Last Control Packet Checked!\n");
  }

  llclose(fd);
  
  restoreFile((char *)name, packet, packet_size, packet_num);

  /*
  deleteFile("test_clone.txt");
  for (int i = 0; i < packet_num; i++)
  {
    // Last Packet
    if (i == (packet_num - 1))
    {
      restoreSimpleFile("test_clone.txt", packet[10], 2);
    }
    else
      restoreSimpleFile("test_clone.txt", packet[i], 127);
  }
  */

  //printf("Cheguei aqui4\n");
  /*
  while (STOP == FALSE)
  {                         
    res = read(fd, buf, 1);
    buf[res] = 0;           
    if (buf[0] == FLAG)
    {
      int i = 0;
      do
      {
        i++;
        read(fd, &buf[i], 1);
      } while (buf[i] != FLAG);
      buf[i + 1] = 0;
      unsigned char *received_data = malloc(MAX_ARRAY_SIZE * sizeof(unsigned char));
      struct parse_results *parse_result = parse_tram(&buf[1], i - 2);
      process_tram_received(parse_result, fd);
      
      res = read(fd, buf, packet_size);
      printf("Received Packet With %d Bytes...\n", res);

      break;
    }
  }
  */

  return 0;
}
