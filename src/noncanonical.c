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
  packet = (unsigned char **) calloc(MAX_PACKET_ELEMS, sizeof(unsigned char *));
  for (int i = 0; i < MAX_PACKET_ELEMS; i++)
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

  fd = llopen(11, RECEIVER);

  // First Control Packet
  unsigned char *control_packet_received = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  llread(fd, (char *)control_packet_received);

  unsigned char *size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *name = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  extract_size_name(control_packet_received, size, name);
  free(control_packet_received);
  long received_size = *((long *)size);
  free(size);
  // Read File Packets Based On Received_Size
  int packet_num;
  if (received_size % packet_size != 0)
    packet_num = received_size / packet_size + 1;
  else
    packet_num = received_size / packet_size;

  unsigned char *tram;
  int stored_packet_size,seq;
  for (int i = 0; i < packet_num; i++)
  {
    tram = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
    stored_packet_size = llread(fd, (char *)tram);
    extract_seq_size_data(tram, &seq, &stored_packet_size, packet[i]);
    free(tram);
  }

  // Last Control Packet
  unsigned char *last_control_packet_received = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  unsigned char *last_size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *last_name = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  llread(fd, (char *)last_control_packet_received);
  extract_size_name(last_control_packet_received, last_size, name);
  long final_received_size = *((long *)last_size);
  free(last_size);

  if (received_size == final_received_size && strcmp((char*)name,(char *) last_name) == 0 && last_control_packet_received[0] == END)
  {
      printf("Last Control Packet Checked!\n");
  }

  llclose(fd);

  restoreFile((char *)name, packet, packet_size, packet_num, final_received_size);

  free(name);
  free(last_control_packet_received);
  free(last_name);

  for (int i = 0; i < MAX_PACKET_ELEMS; i++)
  {
      free(packet[i]);
  }
  free(packet);

  return 0;
}
