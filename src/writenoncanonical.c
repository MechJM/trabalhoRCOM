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
#include <time.h>
#include "tram.h"
#include "state_machine.h"
#include "app_layer.h"
#include "link_layer.h"


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
  set_sigaction();
  struct stat file_data;
  if (stat(argv[2],&file_data) < 0)
  {
    fprintf(stderr, "Couldn't get file data!\n");
  }

  file_size = file_data.st_size;
  
  //char *file_name = "pinguim_clone.gif";
  char file_name[MAX_STR_SIZE + 6];
  char argv_copy[MAX_STR_SIZE];

  strcpy(argv_copy,argv[2]);
  char * actual_file_name = strtok(argv_copy,".");
  sprintf(file_name,"%s_clone.%s",actual_file_name,strtok(NULL,"."));
  //char *file_name = argv[2];
  max_packet_size = atoi(argv[4]);
  packet_size = max_packet_size;
  max_array_size = max_packet_size * 2;
  int fd = 0;
  timeout = 5;

  max_packet_elems = ((int) file_size / max_packet_size) + 1;
  
  //Initialize packet
  packet = (unsigned char **) calloc(max_packet_elems, sizeof(unsigned char *));

  ll = NULL;

  if (argc < 2)
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  unsigned char *fileData = readFile((unsigned char *)argv[2]);

  processFile(fileData);
  free(fileData);

  clock_t begin = clock();

  //int baudRate = B38400;
  int baudRate = 0;

  if (strcmp(argv[3],"B0") == 0) baudRate = B0;
  else if (strcmp(argv[3],"B50") == 0) baudRate = B50;
  else if (strcmp(argv[3],"B75") == 0) baudRate = B75;
  else if (strcmp(argv[3],"B110") == 0) baudRate = B110;
  else if (strcmp(argv[3],"B134") == 0) baudRate = B134;
  else if (strcmp(argv[3],"B150") == 0) baudRate = B150;
  else if (strcmp(argv[3],"B200") == 0) baudRate = B200;
  else if (strcmp(argv[3],"B300") == 0) baudRate = B300;
  else if (strcmp(argv[3],"B600") == 0) baudRate = B600;
  else if (strcmp(argv[3],"B1200") == 0) baudRate = B1200;
  else if (strcmp(argv[3],"B1800") == 0) baudRate = B1800;
  else if (strcmp(argv[3],"B2400") == 0) baudRate = B2400;
  else if (strcmp(argv[3],"B4800") == 0) baudRate = B4800;
  else if (strcmp(argv[3],"B9600") == 0) baudRate = B9600;
  else if (strcmp(argv[3],"B19200") == 0) baudRate = B19200;
  else if (strcmp(argv[3],"B38400") == 0) baudRate = B38400;
  else if (strcmp(argv[3],"B57600") == 0) baudRate = B57600;
  else if (strcmp(argv[3],"B115200") == 0) baudRate = B115200;
  else fprintf(stderr,"Invalid baudrate provided!\n");

  fd = llopen(atoi(argv[1]), TRANSMITTER, baudRate);

  if (fd < 0)
  {
    fprintf(stderr, "llopen failed!\n");
    free(packet);
    return -1;
  }

  int *t_values = calloc(2, sizeof(int));
  t_values[0] = FILE_SIZE;
  t_values[1] = FILE_NAME;
  int *l_values = calloc(2, sizeof(int));
  l_values[0] = sizeof(file_size);
  l_values[1] = strlen(file_name);
  unsigned char **values = (unsigned char **)calloc(2, sizeof(unsigned char *));

  values[0] = (unsigned char *) &file_size;
  values[1] = (unsigned char *) file_name;
  unsigned char *control_packet = generate_control_packet(START, 2, t_values, l_values, values);
  long control_packet_size = 1 + l_values[0] + l_values[1] + 4;
  free(t_values);
  free(l_values);
  free(values);
  
  // First Control Packet
  if (llwrite(fd, (char *)control_packet, control_packet_size) < 0)
  {
    fprintf(stderr, "llwrite failed!\n");
    free(control_packet);
    for (int i = 0; i < max_packet_elems; i++)
    {
        free(packet[i]);
    }
    free(packet);
    return -1;
  }

  //printf("packet_num = %d\n", packet_num);

  // File Packets
  for (int i = 0; i < packet_num; i++)
  {
    unsigned char *data_packet = generate_data_packet(i, packet_size, packet[i]);
    if (llwrite(fd, (char *)data_packet, packet_size + 4) < 0)
    {
      fprintf(stderr, "llwrite failed!\n");
      free(data_packet);
      free(control_packet);
      for (int i = 0; i < max_packet_elems; i++)
      {
          free(packet[i]);
      }
      free(packet);
      return -1;
    }
    free(data_packet);
  }

  // Last Control Packet
  control_packet[0] = END;
  if (llwrite(fd, (char *)control_packet, control_packet_size) < 0)
  {
    fprintf(stderr, "llwrite failed!\n");
    free(control_packet);
    for (int i = 0; i < max_packet_elems; i++)
    {
        free(packet[i]);
    }
    free(packet);
  }

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Execution Time = %f Seconds\n", time_spent);

  free(control_packet);
  llclose(fd);

  for (int i = 0; i < max_packet_elems; i++)
  {
      free(packet[i]);
  }
  free(packet);
  
  return 0;
}
