/* LINK LAYER */

#include "link_layer.h"
#include "app_layer.h"
#include <time.h>

struct termios oldtio, newtio;

int ll_init(char *port, int baudRate, unsigned int timeout, unsigned int numTransmissions)
{

  if (ll == NULL)
  {
    ll = calloc(1,sizeof(link_layer));
  }
    
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

int ll_open_serial_port(int fd, int baudRate)
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
  newtio.c_cflag = baudRate | CS8 | CLOCAL | CREAD;
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

int llopen(int port, int flag, int baudRate)
{
  
  char * actual_port = calloc(12, sizeof(char));
  sprintf(actual_port,"/dev/ttyS%d",port);
  //printf("actual_port: %s\n",actual_port);
  ll_init(actual_port, baudRate, timeout, 1);
  
  int fd = 0;
  if (flag == TRANSMITTER)
  {
    sender = 1;
    setup_initial_values();


    int sent_success = 0;

    fd = ll_open_serial_port(fd, baudRate);
    unsigned char *first_message = generate_su_tram(COMM_SEND_REP_REC, SET, 0);
    int res,result;
    int attempts = 0;
    unsigned char *response;
    while (!sent_success && attempts < TIMEOUT_ATTEMPTS)
    {
      res = write(fd, first_message, NON_INFO_TRAM_SIZE);
      if (res != NON_INFO_TRAM_SIZE)
      {
        fprintf(stderr, "Failed to write in llopen!\n");
        free(first_message);
        return -1;
      }
      alarm(timeout);
      reached_timeout = 0;
      response = receive_tram(fd);
      result = parse_and_process_su_tram(response, fd);
      if (response != NULL) free(response);
      if (result == TIMED_OUT) attempts++;
      else if (result == SEND_NEW_DATA)
      {
        alarm(0);
        sent_success = 1;
      }
      else
      {
        fprintf(stderr, "Wrong result in llopen!\n");
        
        free(first_message);
        return -1;
      }
      
    }
    free(first_message);
    if (attempts == TIMEOUT_ATTEMPTS) return -1;
  }
  else
  {
    sender = 0;
    setup_initial_values();
    fd = ll_open_serial_port(fd, baudRate);
    unsigned char *first_request = receive_tram(fd);
    int result = parse_and_process_su_tram(first_request, fd);

    if (result != DO_NOTHING)
    {
      fprintf(stderr, "Wrong result in llopen!\n");
      return -1;
    }
    free(first_request);
  }
  free(actual_port);
  free(ll);
  printf("Finished the start process!\n");
  return fd;
}

int llwrite(int fd, char *buffer, int length)
{
  int tram_length = length + 6;
  
  unsigned char *data_tram = generate_info_tram(buffer, COMM_SEND_REP_REC, length);
  
  data_tram = byte_stuff(data_tram, &tram_length);

  int res = -1, parse_result;

  int data_sent_success = 0;

  int attempts = 0;
  unsigned char * response;
  
  while (!data_sent_success && attempts < TIMEOUT_ATTEMPTS)
  {
    

    res = write(fd, data_tram, tram_length);
    if (res != tram_length)
    {
      fprintf(stderr, "Failed to write in llwrite!\n");
      return -1;
    }
    alarm(timeout);
    reached_timeout = 0;
    response = receive_tram(fd);

    parse_result = parse_and_process_su_tram(response, fd);
    free(response);
    if (parse_result == SEND_NEW_DATA)
    {
      data_sent_success = 1;
      alarm(0);
    }
    else if (parse_result == DO_NOTHING)
    {
      fprintf(stderr, "S/U tram processing failed in llwrite!\n");
      return -1;
    }
    else if (parse_result == TIMED_OUT) attempts++;
  }

  free(data_tram);
  if (attempts == TIMEOUT_ATTEMPTS)
  {
    fprintf(stderr, "Reached max number of attempts!\n");
    return -1;
  } 

  return res;
}

int llread(int fd, char *buffer)
{
  
  char *actual_data = NULL;
  int data_size;
  unsigned char *data;
  struct parse_results *results;

  while (actual_data == NULL)
  {
    
    data = receive_info_tram(fd, &data_size);
    
    data = byte_unstuff(data, &data_size);
    
    results = parse_info_tram(data, data_size);
    
    actual_data = process_info_tram_received(results, fd);
  }
  free(data);
  free(results);
  
  memcpy(buffer,actual_data,MAX_ARRAY_SIZE);
  free(actual_data);

  data_trams_received = data_trams_received + 1;
  
  return (data_size - 4);
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
    unsigned char *new_tram = generate_su_tram(COMM_SEND_REP_REC, DISC, 0);
    int size = NON_INFO_TRAM_SIZE;
    int res, result;
    int sent_success = 0, attempts = 0;
    while (!sent_success && attempts < TIMEOUT_ATTEMPTS)
    {
      res = write(fd, new_tram, size);
      if (res != NON_INFO_TRAM_SIZE)
      {
        fprintf(stderr, "Failed to write on llclose!\n");
        return -1;
      }
      alarm(timeout);
      reached_timeout = 0;
      unsigned char *response = receive_tram(fd);
      result = parse_and_process_su_tram(response, fd);
      free(response);
      //printf("Result: %d\n",result);
      if (result == TIMED_OUT) attempts++;
      else if (result == DO_NOTHING)
      {
        alarm(0);
        sent_success = 1;
      }
      else
      {
        fprintf(stderr, "Processing failed in llclose for the sender! Result was: %d\n",result);
        return -1;
      }
    }
    sleep(2);
    free(new_tram);
    if (attempts == TIMEOUT_ATTEMPTS) return -1;
  }
  else
  {
    
    unsigned char *end_request = receive_tram(fd);
    
    int result = parse_and_process_su_tram(end_request, fd);
    
    if (result != DO_NOTHING)
    {
      fprintf(stderr, "Processing failed in llclose for the receiver!\n");
      return -1;
    }
    
    free(end_request);
    unsigned char *acknowledgment = receive_tram(fd);
    
    result = parse_and_process_su_tram(acknowledgment, fd);
    
    if (result != DO_NOTHING)
    {
      fprintf(stderr, "Processing failed in llclose for the receiver!\n");
      return -1;
    }
    
    free(acknowledgment);
  }
  ll_close_serial_port(fd);
  return 1;
}