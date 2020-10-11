/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "tram.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

void checkBuffer(int fd, char buf[255])
{
  // Check Buff
  if (buf[0] == '0' && buf[1] == '0') // ...
  {
    // Retransmit?
    write(fd, buf, 255);
  }
  return;
}

int main(int argc, char **argv)
{
  setup_rs();
  //int fd,c, res;
  int fd, res;
  struct termios oldtio, newtio;
  unsigned char buf[255];

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

  while (STOP == FALSE)
  {                           /* loop for input */
    res = read(fd, buf, 1); /* returns after 5 chars have been input */
    buf[res] = 0;             /* so we can printf... */
    //checkBuffer(fd, buf);
    if (buf[0] == FLAG)
    {
      int i = 0;
      do
      {
        i++;
        read(fd,&buf[i],1);

      } while (buf[i] != FLAG);
      buf[i+1] = 0;
      unsigned char * received_data = malloc(255*sizeof(unsigned char));
      int parse_result = parse_tram(&buf[1],i-2,received_data);
      process_tram_received(parse_result,NULL,0,fd);
      //TODO
      break;
    }
    /*
    printf(":%s:%d\n", buf, res);
    if (buf[0] == 'z')
      STOP = TRUE;*/
  }

  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
  */

  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
  return 0;
}
