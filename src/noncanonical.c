/* Non-Canonical Input Processing */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tram.h"
#include "state_machine.h"
#include "link_layer.h"
#include "app_layer.h"

#define FALSE 0

volatile int STOP = FALSE;

int main(int argc, char **argv)
{

  int fd = 0;
  timeout = 1;
  ll = NULL;
  packet_size = atoi(argv[3]);
  max_packet_size = packet_size;

  //Initialize packet
  packet = (unsigned char **)calloc(MAX_PACKET_ELEMS, sizeof(unsigned char *));
  for (int i = 0; i < MAX_PACKET_ELEMS; i++)
  {
    packet[i] = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  }

  if (argc < 2)
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  //int baudRate = B38400;
  int baudRate = atoi(argv[2]);
  fd = llopen(atoi(argv[1]), RECEIVER, baudRate);

  if (fd < 0)
  {
    fprintf(stderr, "llopen failed!\n");
    for (size_t i = 0; i < MAX_PACKET_ELEMS; i++)
    {
      free(packet[i]);
    }

    free(packet);
    return -1;
  }

  // First Control Packet
  unsigned char *control_packet_received = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  llread(fd, (char *)control_packet_received);

  clock_t begin = clock();

  unsigned char *size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *name = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  extract_size_name(control_packet_received, size, name);

  unsigned char *expected_final_control = control_packet_received;
  expected_final_control[0] = END;

  long received_size = *((long *)size);
  free(size);
  // Read File Packets Based On Received_Size
  int packet_num;
  if (received_size % packet_size != 0)
    packet_num = received_size / packet_size + 1;
  else
    packet_num = received_size / packet_size;

  unsigned char *tram = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));
  int stored_packet_size, seq;

  //Main reception loop
  int i = 0;
  while (strcmp((char *)tram, (char *)expected_final_control) != 0)
  {
    stored_packet_size = llread(fd, (char *)tram);
    if (tram[0] != 1)
      break;
    extract_seq_size_data(tram, &seq, &stored_packet_size, packet[i]);
    i++;
  }

  // Last Control Packet
  unsigned char *last_size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *last_name = (unsigned char *)calloc(MAX_ARRAY_SIZE, sizeof(unsigned char));

  extract_size_name(tram, last_size, name);
  long final_received_size = *((long *)last_size);
  free(last_size);

  if (received_size == final_received_size && strcmp((char *)name, (char *)last_name) == 0 && tram[0] == END)
  {
    printf("Last Control Packet Checked!\n");
  }

  llclose(fd);

  restoreFile((char *)name, packet, packet_size, packet_num, final_received_size);

  free(name);
  free(last_name);
  free(tram);
  free(control_packet_received);

  for (int i = 0; i < MAX_PACKET_ELEMS; i++)
  {
    free(packet[i]);
  }
  free(packet);

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Execution Time = %f Seconds\n", time_spent);

  return 0;
}
