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

void sigalrm_handler(int signo)
{
  if (signo != SIGALRM) fprintf(stderr, "This handler shouldn't have been called.\n");
  need_to_wait = 0;
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
  srand(time(NULL));
  set_sigaction();
  int fd = 0;
  timeout = 1;
  ll = NULL;
  packet_size = atoi(argv[3]);
  max_packet_size = packet_size;
  max_array_size = max_packet_size * 2;

  fer = atoi(argv[4]);
  t_prop = atoi(argv[5]);
  

  if (argc < 2)
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  //int baudRate = B38400;
  int baudRate = atoi(argv[2]);

  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);

  fd = llopen(atoi(argv[1]), RECEIVER, baudRate);

  if (fd < 0)
  {
    fprintf(stderr, "llopen failed!\n");
    return -1;
  }

  

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

  llclose(fd);
  struct timespec end_time;
  clock_gettime(CLOCK_REALTIME, &end_time);
  double sTime = start_time.tv_sec + start_time.tv_nsec * 1e-9;
  double eTime = end_time.tv_sec + end_time.tv_nsec * 1e-9;
  double final_time = eTime - sTime;
  //Writing results to csv
  FILE* csv = fopen("results_reader.csv","w");
  fprintf(csv,"%d,%d,%d,%ld,%f\n",fer,t_prop,baudRate,max_packet_size,final_time);
  fclose(csv);
  //printf("Execution Time = %.6lf\n", eTime - sTime);

  restoreFile((char *)name, packet, packet_size, packet_num, final_received_size);
  
  
  
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
