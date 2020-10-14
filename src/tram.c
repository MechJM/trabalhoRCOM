#include "tram.h"

void setup_rs()
{
    r = 1;
    s = 0;
}

unsigned char *generate_info_tram(unsigned char *data, unsigned char address, int array_size)
{
    unsigned char *tram = malloc((6 + array_size) * sizeof(unsigned char));

    tram[0] = FLAG;
    tram[1] = address;

    unsigned char actual_control = INFO_CTRL;

    if (s > 0)
    {
        actual_control |= S_MASK;
        s--;
    }
    else
        s++;

    tram[2] = actual_control;
    tram[3] = address ^ actual_control;

    unsigned char bcc2 = 0x00;

    for (int i = 4; i < (array_size + 4); i++)
    {
        tram[i] = data[i - 4];
        bcc2 ^= tram[i];
    }

    tram[4 + array_size] = bcc2;
    tram[5 + array_size] = FLAG;

    return tram;
}

unsigned char *generate_su_tram(unsigned char address, unsigned char control)
{
    unsigned char *tram = malloc(5 * sizeof(unsigned char));
    tram[0] = FLAG;
    tram[1] = address;

    unsigned char actual_control = control;

    if (control == RR || control == REJ)
    {
        if (r > 0)
        {
            actual_control |= R_MASK;
            r--;
        }
        else
            r++;
    }

    tram[2] = actual_control;
    tram[3] = address ^ actual_control;
    tram[4] = FLAG;

    return tram;
}

int parse_tram(unsigned char *tram, int tram_size, unsigned char *data_parsed)
{
    if ((tram[0] != COMM_SEND_REP_REC && tram[0] != COMM_REC_REP_SEND)                                                                                                                                                  //Checks if the second byte matches one of the possible values for the address field
        || (tram[1] != INFO_CTRL && tram[1] != (INFO_CTRL | S_MASK) && tram[1] != SET && tram[1] != DISC && tram[1] != UA && tram[1] != RR && tram[1] != (RR | R_MASK) && tram[1] != REJ && tram[1] != (REJ | R_MASK))) //Checks if the third byte matches one of the possible values for the control field
        return WRONG_HEADER;

    switch (tram[1])
    {
    case SET:
    {
        printf("Request to start communication received. Acknowledging.\n");
        return START_COMMUNICATION;
    }
    case UA:
    {
        printf("Request to start/end communication was acknowledged.\n");
        return ACKNOWLEDGE_START;
    }
    case INFO_CTRL:
    {
        for (int i = 3; i < (tram_size + 3 - 4); i++)
        {
            data_parsed[i - 3] = tram[i];
        }
        printf("Data tram received.\n");
        return DATA_RECEIVED;
    }
    case (INFO_CTRL | S_MASK):
    {
        for (int i = 3; i < (tram_size + 3 - 4); i++)
        {
            data_parsed[i - 3] = tram[i];
        }
        printf("Data tram received.\n");
        return DATA_RECEIVED;
    }
    case DISC:
    {
        printf("Connection ended.\n");
        return END_COMMUNICATION;
    }
    case RR:
    {
        printf("Data was sent without issues. Positive acknowledgment.\n");
        return NO_ISSUE_DATA;
    }
    case (RR | R_MASK):
    {
        printf("Data was sent without issues. Positive acknowledgment.\n");
        return NO_ISSUE_DATA;
    }
    case REJ:
    {
        printf("Data sent had issues. Negative acknowledgment.\n");
        return ISSUE_DATA;
    }
    case (REJ | R_MASK):
    {
        printf("Data sent had issues. Negative acknowledgment.\n");
        return ISSUE_DATA;
    }
    default:
        fprintf(stderr, "Invalid control byte! Value: %s\n", &tram[1]);
    }
    //TODO VERIFY BCCs
    return 0;
}

void process_tram_received(int parse_result, unsigned char *data_to_be_sent, int data_size, int port)
{
    unsigned char *response;
    int response_size = 0;

    switch (parse_result)
    {
    case START_COMMUNICATION:
    {
        response = generate_su_tram(COMM_SEND_REP_REC, UA);
        response_size = 5;
        break;
    }
    case ACKNOWLEDGE_START:
    {
        response = generate_info_tram(data_to_be_sent, COMM_SEND_REP_REC, data_size);
        response_size = data_size;
        break;
    }
    case WRONG_HEADER:
    {
        return;
    }
    case DATA_RECEIVED:
    {
        response = generate_su_tram(COMM_SEND_REP_REC, RR);
        response_size = 5;
        break;
    }
    case END_COMMUNICATION:
    {
        response = generate_su_tram(COMM_SEND_REP_REC, DISC);
        response_size = 5;
        break;
    }
    case NO_ISSUE_DATA:
    {
        //TODO
        break;
    }
    case ISSUE_DATA:
    {
        //TODO
        break;
    }
    case INTEGRITY_FAILED:
    {
        //TODO
        break;
    }
    default:
        fprintf(stderr, "Invalid parse result! Value: %d\n", parse_result);
    }

    //TODO FINISH THIS FUNCTION
    int res = write(port, response, response_size);
    printf("%d Bytes Written\n", res);
}

void byte_stuff(unsigned char * tram)
{}

void byte_unstuff(unsigned char * tram)
{}