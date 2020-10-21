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

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS11"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define PORT_NAME "/dev/ttyS"
#define MAX_TRAM_SIZE 2

volatile int STOP = FALSE;

void sigalrm_handler(int signo)
{
  if (signo != SIGALRM)
    fprintf(stderr, "This signal handler shouldn't have been called. signo: %d\n", signo);
  timeout = 0;
}

/* LINK LAYER */
struct termios oldtio, newtio;
typedef struct
{
  char *port;
  int baudRate;
  unsigned int sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  char frame[MAX_TRAM_SIZE];
} link_layer;
static link_layer *ll = NULL;
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
void llclose()
{
  // TODO
  // SEND DISC
  // READ DISC
  // SEND UA
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
/* -- */

/* APP LAYER FILE HANDLING */
long int file_size;
int packet_size = 127;
int packet_num;
unsigned char *readFile(unsigned char *fileName)
{
  printf("Reading File <%s>\n", fileName);
  FILE *f;
  struct stat metadata;
  unsigned char *fileData;
  if ((f = fopen((char *)fileName, "rb")) == NULL)
  {
    perror("Error Read File!");
  }
  stat((char *)fileName, &metadata);
  file_size = metadata.st_size;
  printf("File Size =  %ld Bytes\n", file_size);
  fileData = (unsigned char *)malloc(file_size);
  fread(fileData, sizeof(unsigned char), file_size, f);
  fclose(f);
  return fileData;
}
unsigned char *splitFileData(unsigned char *fileData, int x, int packet_size)
{
  unsigned char *packet_temp = (unsigned char *)malloc(packet_size);
  int i;
  int j;
  for (j = 0, i = x; i < packet_size; i++, j++)
  {
    packet_temp[j] = fileData[i];
  }
  return packet_temp;
}
void savePackets(unsigned char *packet[], unsigned char *fileData)
{
  for (int i = 0; i < packet_num; i++)
  {
    packet[i] = splitFileData(fileData, packet_size * i, i * packet_size + packet_size);
    printf("Packet[%d] Saved!\n", i);
  }
}
void restoreFile(char *fileName, unsigned char *packet[], int packet_num)
{
  printf("Restoring File...\n");
  FILE *f = fopen((char *)fileName, "wb+");
  for (int i = 0; i < packet_num; i++)
  {
    fwrite((void *)packet[i], 1, packet_size, f);
  }
  printf("File Restored!\n");
  fclose(f);
}
void restoreSimpleFile(char *fileName, unsigned char *fileData, long int file_size)
{
  FILE *f = fopen((char *)fileName, "wb+");
  fwrite((void *)fileData, 1, file_size, f);
  printf("New File Created!\n");
  fclose(f);
}
void processFile(unsigned char *fileData)
{
  if (file_size % packet_size != 0)
  {
    packet_num = file_size / packet_size + 1;
  }
  else
  {
    packet_num = file_size / packet_size;
  }
  packet = calloc(packet_num, packet_size);
  printf("Created %d Packets...\n", packet_num);
  savePackets(packet, fileData);
  printf("Packets Ready To Be Sent!\n");
}
/* -- */

int main(int argc, char **argv)
{
  packet_size = 127;
  sender = 1;
  int fd = 0;

  timeout = 1;
  int n = 5;
  int numTransmissions = 1;

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

  char *restoredFileName = "restoreFile.gif";
  restoreFile(restoredFileName, packet, packet_num);
  */

  fd = ll_open_serial_port(fd);

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  struct sigaction action;
  action.sa_handler = sigalrm_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if (sigaction(SIGALRM, &action, NULL) < 0)
    fprintf(stderr, "Couldn't install signal handler for SIGALRM.\n");

  unsigned char *tram = generate_su_tram(COMM_SEND_REP_REC, SET);

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
  }

  llclose();
  ll_close_serial_port(fd);
  return 0;
}