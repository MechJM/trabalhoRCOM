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
  sender = 0;
  setup_initial_values();
  data_trams_received = 0;
  int fd = 0;
  timeout = 1;
  ll = NULL;
  packet_size = 127;

  if ((argc < 2) ||
      ((strcmp("/dev/ttyS10", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  fd = llopen(11, RECEIVER);

  // First Control Packet
  unsigned char *control_packet_received = (unsigned char *)calloc(255, sizeof(unsigned char));
  llread(fd, (char *)control_packet_received);
  
  unsigned char *size = (unsigned char *)calloc(8, sizeof(unsigned char));
  unsigned char *name = (unsigned char *)calloc(255, sizeof(unsigned char));
  extract_size_name(control_packet_received, size, name);
  long received_size =  *((long*)size);
  
  received_size=received_size; //just so the compiler doesn't complain
  // File Packets
  int packet_num = 87;
  for (int i = 0; i < packet_num; i++)
  {
    llread(fd, (char *)packet[i]);
  }

  // Last Control Packet
  llread(fd, (char *)control_packet_received);

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
      unsigned char *received_data = malloc(255 * sizeof(unsigned char));
      struct parse_results *parse_result = parse_tram(&buf[1], i - 2);
      process_tram_received(parse_result, fd);
      
      res = read(fd, buf, packet_size);
      printf("Received Packet With %d Bytes...\n", res);

      break;
    }
  }
  */

  llclose(fd);

  restoreFile("pinguim_clone.gif", packet, packet_size, packet_num);

  return 0;
}
