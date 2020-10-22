/* LINK LAYER */


#include "link_layer.h"

struct termios oldtio, newtio;


int ll_init(char *port, int baudRate, unsigned int timeout, unsigned int numTransmissions)
{
  if (ll == NULL)
    ll = malloc(sizeof(link_layer));
  else
    printf("Link Layer Already Initialized\n");
  ll->port = port;
  ll->baudRate = baudRate;
  ll->timeout = timeout;
  ll->numTransmissions = numTransmissions;
  ll->sequenceNumber = 0;
  printf("Link Layer Initialized!\n");
  return 0;
}

int ll_open_serial_port(int fd)
{
  fd = open(ll->port, O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(ll->port);
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

  newtio.c_cc[VTIME] = ll->timeout * 10; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 5;                 /* blocking read until 5 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) proximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");
  return fd;
}

int llopen(int fd, int flag)
{
  flag=flag; //Just a placeholder to allow the program to compile
  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */
  fd = ll_open_serial_port(fd);
  // TODO
  // SEND SET
  // READ UA
  return fd;
}

void ll_close_serial_port(int fd)
{
  if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }
  close(fd);
}

int llclose(int fd)
{
  if (sender)
    {
        unsigned char * new_tram = generate_su_tram(COMM_SEND_REP_REC, DISC);
        int size = NON_INFO_TRAM_SIZE;
        int res = write(fd, new_tram, size);
        if (res != NON_INFO_TRAM_SIZE)
        {
            fprintf(stderr, "Failed to write on llclose!\n");
            return -1;
        } 

        unsigned char * response = receive_tram(fd);
        int result = parse_and_process_su_tram(response, fd);
        if (result != DO_NOTHING)
        {
            fprintf(stderr, "Processing failed!\n");
            return -1;
        } 
    }
    else 
    {
        unsigned char * end_request = receive_tram(fd);
        int result = parse_and_process_su_tram(end_request, fd);
        if (result != DO_NOTHING)
        {
            fprintf(stderr, "Processing failed!\n");
            return -1;
        }

        unsigned char * acknowledgment = receive_tram(fd);
        result = parse_and_process_su_tram(acknowledgment, fd);
        if (result != DO_NOTHING)
        {
            fprintf(stderr, "Processing failed!\n");
            return -1;
        }
        
    }
    ll_close_serial_port(fd);
    return 1;
}