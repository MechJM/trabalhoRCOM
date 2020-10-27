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
  if (signo != SIGALRM) fprintf(stderr, "This signal handler shouldn't have been called. signo: %d\n", signo);
  reached_timeout = 1;
}

void set_sigaction()
{
  struct sigaction action;
  action.sa_handler = sigalrm_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if (sigaction(SIGALRM, &action, NULL) < 0) fprintf(stderr, "Couldn't install signal handler for SIGALRM.\n");
}

int main(int argc, char **argv)
{
  long file_size = 10968;
  char *file_name = "pinguim_clone.gif";
  packet_size = 127;
  int fd = 0;
  timeout = 1;
  //Initialize packet
  packet = (unsigned char **) calloc(255, sizeof(unsigned char *));
  for (int i = 0; i < 255; i++)
  {
      packet[i] = (unsigned char *) calloc(255, sizeof(unsigned char));
  }

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

  fd = llopen(10, TRANSMITTER);

  set_sigaction();

  int *t_values = calloc(1, sizeof(int));
  t_values[0] = FILE_SIZE;
  t_values[1] = FILE_NAME;
  int *l_values = calloc(1, sizeof(int));
  l_values[0] = sizeof(file_size);
  l_values[1] = strlen(file_name);
  unsigned char **values = (unsigned char **)calloc(1, sizeof(unsigned char *));
  //values[0] = (unsigned char *)calloc(file_size, sizeof(unsigned char));
  values[0] = (unsigned char *) &file_size;
  values[1] = (unsigned char *)file_name;
  unsigned char *control_packet = generate_control_packet(START, 2, t_values, l_values, values);

  /*printf("control packet:\n");
  for (int i = 0; i < (1 + 4 + l_values[0] + l_values[1]); i++)
  {
    printf("%x ",control_packet[i]);
  }
  
  printf("\n");*/
  
  // First Control Packet
  set_sigaction();

  llwrite(fd, (char *)control_packet, packet_size);

  // File Packets
  for (int i = 0; i < packet_num; i++)
  {
    /*
    printf("packet[%d] being sent:\n",i);
    for (size_t j = 0; j < 255; j++)
    {
      printf("%x ",packet[i][j]);
    }
    
    printf("\n");*/
    unsigned char *data_packet = generate_data_packet(i, 255, packet[i]);
    /*printf("data packet generated for packet[%d]:\n",i);
    for (size_t j = 0; j < 270; j++)
    {
      printf("%x ",data_packet[j]);
    }
    printf("\n");
    */
    llwrite(fd, (char *)data_packet, packet_size + 4);
  }

  // Last Control Packet
  control_packet[0] = END;
  llwrite(fd, (char *)control_packet, packet_size);

  /*
  unsigned char * arr = calloc(5,sizeof(unsigned char));
  arr[0] = 0x01;
  arr[1] = 0x02;
  arr[2] = 0x03;
  arr[3] = 0x04;
  arr[4] = 0x05;
  arr = translate_array(arr,-2,5,1);
  for (size_t i = 0; i < 5; i++)
  {
    printf("%x ",arr[i]);
  }
  printf("\n");*/
  /*
  unsigned char * packet_3 = packet[2];
  
  printf("Before stuffing:\n");
  for (int i = 0; i < packet_size; i++)
  {
    printf("%x ",packet_3[i]);
  }
  printf("\n");
  int size = packet_size;
  packet_3 = byte_stuff(packet_3,&size);
  printf("After stuffing:\n");
  for (int i = 0; i < size; i++)
  {
    printf("%x ",packet_3[i]);
  }
  printf("\n");
  */

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