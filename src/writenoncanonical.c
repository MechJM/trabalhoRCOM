/* Non-Canonical Input Processing */

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
#include "tram.h"
#include "state_machine.h"
#include "app_layer.h"
#include "link_layer.h"

#define BAUDRATE B38400

void sigalrm_handler(int signo)
{
  if (signo != SIGALRM)
    fprintf(stderr, "This signal handler shouldn't have been called. signo: %d\n", signo);
  timeout = 0;
}

void set_sigaction()
{
  struct sigaction action;
  action.sa_handler = sigalrm_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if (sigaction(SIGALRM, &action, NULL) < 0)
    fprintf(stderr, "Couldn't install signal handler for SIGALRM.\n");
}

int main(int argc, char **argv)
{
  setup_initial_values();
  packet_size = 127;
  sender = 1;
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

  unsigned char *fileData = readFile((unsigned char *)argv[2]);

  processFile(fileData);

  ll_init(argv[1], BAUDRATE, timeout, numTransmissions);

  fd = llopen(fd, TRANSMITTER);

  set_sigaction();

  // Ciclo para todas as packets aqui
  llwrite(fd, packet[0], packet_size);

  //unsigned char *tram = generate_su_tram(COMM_SEND_REP_REC, SET);
  /*
  for (unsigned int j = 0; j < ll->numTransmissions; j++)
  {
    int res = write(fd, tram, n);
    printf("Writting %d Bytes\n", res);

    alarm(ll->numTransmissions);
    printf("Attempting to establish connection...\n");

    unsigned char *response = malloc(255 * sizeof(unsigned char));
    while (ll->timeout != 0)
    {
      read(fd, response, 1);
      if (response[0] == FLAG)
      {
        int i = 0;
        do
        {
          i++;
          read(fd, &response[i], 1);
        } while (response[i] != FLAG && ll->timeout);
        response[i + 1] = 0;
        
        struct parse_results *parse_result = parse_tram(&response[1], i - 2);
        process_tram_received(parse_result, fd);
        
        // loop que envia packet[i] e verifica trama recebida do recetor

        ll->numTransmissions--;
        break;
      }
    }
    ll->timeout--;
  }*/

  llclose(fd);

  return 0;
}