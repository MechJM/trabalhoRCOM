/* LINK LAYER */

#include "link_layer.h"
#include "app_layer.h"

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

int llopen(int flag)
{
  int fd = 0;
  if (flag == TRANSMITTER)
  {
    fd = ll_open_serial_port(fd);
    unsigned char *first_message = generate_su_tram(COMM_SEND_REP_REC, SET, 0);
    int res = write(fd, first_message, NON_INFO_TRAM_SIZE);
    if (res != NON_INFO_TRAM_SIZE)
    {
      fprintf(stderr, "Failed to write in llopen!\n");
      return -1;
    }
    unsigned char *response = receive_tram(fd);
    int result = parse_and_process_su_tram(response, fd);
    if (result != SEND_NEW_DATA)
    {
      fprintf(stderr, "Wrong result in llopen!\n");
      return -1;
    }
  }
  else
  {
    fd = ll_open_serial_port(fd);
    unsigned char *first_request = receive_tram(fd);
    int result = parse_and_process_su_tram(first_request, fd);

    if (result != DO_NOTHING)
    {
      fprintf(stderr, "Wrong result in llopen!\n");
      return -1;
    }
  }
  printf("Finished the start process!\n");
  return fd;
}

int llwrite(int fd, char *buffer, int length)
{

  int tram_length = length + 6;
  unsigned char *data_tram = generate_info_tram(buffer, COMM_SEND_REP_REC, length);
  /*printf("Data being sent before stuffing:\n");
  for (int i = 0; i < length + 6; i++)
  {
    printf("%x ",data_tram[i]);
  }
  printf("\n");
  */
  data_tram = byte_stuff(data_tram, &tram_length);
  /*printf("Data being sent after stuffing:\n");
  for (int i = 0; i < length + 6; i++)
  {
    printf("%x ",data_tram[i]);
  }
  printf("\n");
  */
  int res, parse_result;

  int data_sent_success = 0;

  while (!data_sent_success)
  {

    res = write(fd, data_tram, tram_length);
    if (res != tram_length)
    {
      fprintf(stderr, "Failed to write in llwrite!\n");
      return -1;
    }

    unsigned char *response = receive_tram(fd);
    /*
    printf("Response: ");
    for (int i = 0; i < 3; i++)
    {
      printf("%x ",response[i]);
    }
    
    printf("\n");*/

    parse_result = parse_and_process_su_tram(response, fd);
    if (parse_result == SEND_NEW_DATA)
      data_sent_success = 1;
    else if (parse_result == DO_NOTHING)
    {
      fprintf(stderr, "S/U tram processing failed in llwrite!\n");
      return -1;
    }
  }

  return res;
}

int llread(int fd, char *buffer)
{
  char *actual_data = NULL;
  int data_size;
  while (actual_data == NULL)
  {
    unsigned char *data = receive_info_tram(fd, &data_size);
    data = byte_unstuff(data, &data_size);
    /*
    printf("Data received after :\n");
    for (int i = 0; i < data_size; i++)
    {
      printf("%x ",data[i]);
    }
    printf("\n");*/
    struct parse_results *results = parse_info_tram(data, data_size);
    actual_data = process_info_tram_received(results, fd);
   
  }
  //printf("actual data:\n");
  for (int i = 0; i < 255; i++)
  {
    //printf("%x ",(unsigned char) actual_data[i]);
    buffer[i] = actual_data[i];
  }
  //printf("\n");
  //buffer = actual_data;
  buffer = buffer; //only here because otherwise the compiler throws an error about an unused parameter
  data_trams_received = data_trams_received + 1;
  return (data_size - 4);
}

/*
int llwrite(int fd, unsigned char *packet, int packet_size)
{
  packet_size = 127;
  unsigned char *tram_i = generate_info_tram(packet, COMM_SEND_REP_REC, packet_size);
  int new_packet_size = 127 + 6;
  byte_stuff(tram_i, &new_packet_size);
  
  printf("Data being sent: ");
  for (int i = 0; i < new_packet_size; i++)
  {
    printf("%x ", tram_i[i]);
  }
  
  printf("\n");
  int res = write(fd, tram_i, new_packet_size);
  if (res != (new_packet_size))
  {
    fprintf(stderr, "Failed to write in llwrite!\n");
    return -1;
  }
  printf("I Tram Sent...\nWaiting For RR...\n");
  unsigned char * response = receive_tram(fd);
  int result = parse_and_process_su_tram(response,fd);
  if (result == RESEND_DATA)
  {
    res = write(fd, tram_i, new_packet_size);
    if (res != (new_packet_size))
    {
      fprintf(stderr, "Failed to write in llwrite!\n");
      return -1;
    }
  }
  
  return res;
}

int llread(int fd, char * buffer)
{

  printf("I Tram Received!\n");
  int size;
  unsigned char *request = receive_info_tram(fd, &size);

  byte_unstuff(request, &size);

  struct parse_results *result = parse_info_tram(request, size);

  process_info_tram_received(result, fd);
  

  return size;
}
*/

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
    unsigned char *new_tram = generate_su_tram(COMM_SEND_REP_REC, DISC, 0);
    int size = NON_INFO_TRAM_SIZE;
    int res = write(fd, new_tram, size);
    if (res != NON_INFO_TRAM_SIZE)
    {
      fprintf(stderr, "Failed to write on llclose!\n");
      return -1;
    }

    unsigned char *response = receive_tram(fd);
    int result = parse_and_process_su_tram(response, fd);
    //printf("Result: %d\n",result);
    if (result != DO_NOTHING)
    {
      fprintf(stderr, "Processing failed!\n");
      return -1;
    }
  }
  else
  {
    unsigned char *end_request = receive_tram(fd);
    int result = parse_and_process_su_tram(end_request, fd);
    if (result != DO_NOTHING)
    {
      fprintf(stderr, "Processing failed!\n");
      return -1;
    }

    unsigned char *acknowledgment = receive_tram(fd);
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
