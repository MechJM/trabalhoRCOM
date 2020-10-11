/*Non-Canonical Input Processing*/

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

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS11"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

int timeout;

void sigalrm_handler(int signo)
{
  if (signo != SIGALRM) fprintf(stderr,"This signal handler shouldn't have been called. signo: %d\n",signo);
  timeout = 0;
}

int main(int argc, char **argv)
{
  setup_rs();

  //int fd,c, res;
  int fd;//, res;
  struct termios oldtio, newtio;
  // TP1
  // Class 2
  timeout = 1;
  //unsigned char * buf_temp = generate_su_tram(COMM_SEND_REP_REC,SET);
  
  //int n = sizeof(buf_temp)/sizeof(unsigned char);
  int n = 5;
  
  //unsigned char buf[n];
  //int i; // sum = 0, speed = 0;

  if ((argc < 2) ||
      ((strcmp("/dev/ttyS10", argv[1]) != 0) &&
       (strcmp("/dev/ttyS11", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(argv[1]);
    exit(-1);
  }

  if (tcgetattr(fd, &oldtio) == -1)
  { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 5;  /* blocking read until 5 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  struct sigaction action;
  action.sa_handler = sigalrm_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if (sigaction(SIGALRM,&action,NULL) < 0) fprintf(stderr,"Couldn't install signal handler for SIGALRM.\n");


  unsigned char * tram = generate_su_tram(COMM_SEND_REP_REC,SET);

  for (int j = 0; j < 3; j++)
  {
    write(fd, tram, n);
    alarm(3);
    printf("Attempting to establish connection.\n");
    
    unsigned char * response = malloc(255*sizeof(unsigned char));
    while(timeout)
    {
      
      read(fd,response,1);
      
      if (response[0] == FLAG)
      {
        
        int i = 0;
        do
        {
          i++;
          read(fd,&response[i],1);
        } while (response[i] != FLAG && timeout);
        response[i+1] = 0;
        unsigned char * received_data = malloc(255*sizeof(unsigned char));
        int parse_result = parse_tram(&response[1],i-2,received_data);
        process_tram_received(parse_result,received_data,fd);
        //TODO
        j = 3;
        break;
      }
      
    }
    
    timeout = 1;
  }

  

  /* 
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar 
    o indicado no gui�o 
  */

  if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}
