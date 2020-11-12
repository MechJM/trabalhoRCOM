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
  max_array_size = max_packet_size * 2;

  

  if (argc < 2)
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  //int baudRate = B38400;
  int baudRate = 0;

  if (strcmp(argv[2],"B0") == 0) baudRate = B0;
  else if (strcmp(argv[2],"B50") == 0) baudRate = B50;
  else if (strcmp(argv[2],"B75") == 0) baudRate = B75;
  else if (strcmp(argv[2],"B110") == 0) baudRate = B110;
  else if (strcmp(argv[2],"B134") == 0) baudRate = B134;
  else if (strcmp(argv[2],"B150") == 0) baudRate = B150;
  else if (strcmp(argv[2],"B200") == 0) baudRate = B200;
  else if (strcmp(argv[2],"B300") == 0) baudRate = B300;
  else if (strcmp(argv[2],"B600") == 0) baudRate = B600;
  else if (strcmp(argv[2],"B1200") == 0) baudRate = B1200;
  else if (strcmp(argv[2],"B1800") == 0) baudRate = B1800;
  else if (strcmp(argv[2],"B2400") == 0) baudRate = B2400;
  else if (strcmp(argv[2],"B4800") == 0) baudRate = B4800;
  else if (strcmp(argv[2],"B9600") == 0) baudRate = B9600;
  else if (strcmp(argv[2],"B19200") == 0) baudRate = B19200;
  else if (strcmp(argv[2],"B38400") == 0) baudRate = B38400;
  else if (strcmp(argv[2],"B57600") == 0) baudRate = B57600;
  else if (strcmp(argv[2],"B115200") == 0) baudRate = B115200;
  else fprintf(stderr,"Invalid baudrate provided!\n");

  fd = llopen(atoi(argv[1]), RECEIVER, baudRate);

  if (fd < 0)
  {
    fprintf(stderr, "llopen failed!\n");
    return -1;
  }

  clock_t begin = clock();

  // First Control Packet
  unsigned char *control_packet_received = (unsigned char *)calloc(max_array_size, sizeof(unsigned char));
  llread(fd, (char *)control_packet_received);

  unsigned char *size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *name = (unsigned char *)calloc(MAX_STR_SIZE, sizeof(unsigned char));
  extract_size_name(control_packet_received, size, name);

  unsigned char *expected_final_control = control_packet_received;
  expected_final_control[0] = END;

  long received_size = *((long *)size);

  max_packet_elems = ((int) received_size / max_packet_size) + 1;

  //Initialize packet
  packet = (unsigned char **)calloc(max_packet_elems, sizeof(unsigned char *));
  for (int i = 0; i < max_packet_elems; i++)
  {
    packet[i] = (unsigned char *)calloc(max_array_size, sizeof(unsigned char));
  }


  free(size);
  // Read File Packets Based On Received_Size
  int packet_num;
  if (received_size % packet_size != 0)
    packet_num = received_size / packet_size + 1;
  else
    packet_num = received_size / packet_size;

  unsigned char *tram = (unsigned char *)calloc(max_array_size, sizeof(unsigned char));
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
    printf("Packet Restore = %d%%\n", (100 * i / packet_num));
  }

  // Last Control Packet
  unsigned char *last_size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *last_name = (unsigned char *)calloc(MAX_STR_SIZE, sizeof(unsigned char));

  extract_size_name(tram, last_size, name);
  long final_received_size = *((long *)last_size);
  free(last_size);

  if (received_size == final_received_size && strcmp((char *)name, (char *)last_name) == 0 && tram[0] == END)
  {
    printf("Last Control Packet Checked!\n");
  }

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Execution Time = %f Seconds\n", time_spent);

  restoreFile((char *)name, packet, packet_size, packet_num, final_received_size);
  
  llclose(fd);
  
  free(name);
  free(last_name);
  free(tram);
  free(control_packet_received);

  for (int i = 0; i < max_packet_elems; i++)
  {
    free(packet[i]);
  }
  free(packet);

  return 0;
}
