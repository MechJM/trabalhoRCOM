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
  int numTransmissions = 1;
  ll = NULL;

  if ((argc < 2) ||
      ((strcmp("/dev/ttyS10", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  ll_init(argv[1], BAUDRATE, timeout, numTransmissions);

  fd = llopen(fd, RECEIVER);
  packet = malloc(sizeof(unsigned char *));
  *packet = calloc(255, sizeof(unsigned char));

  llread(fd);
  llread(fd);
  restoreSimpleFile("teste_read.txt", packet[1], 127);

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

  return 0;
}
