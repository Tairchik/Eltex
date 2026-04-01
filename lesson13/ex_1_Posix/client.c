#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

int main()
{  
    mqd_t mq_s2c = mq_open(SERVER_TO_CLIENT, O_RDONLY);
    if (mq_s2c == -1) 
    {
        perror("SERVER_TO_CLIENT mq_open");
        exit(EXIT_FAILURE);
    }
    mqd_t mq_c2s = mq_open(CLIENT_TO_SERVER, O_WRONLY);
    if (mq_c2s == -1) 
    {
        perror("CLIENT_TO_SERVER  mq_open");
        exit(EXIT_FAILURE);
    }
    char message_in[MSGLEN];
    if (mq_receive(mq_s2c, message_in, MSGLEN, NULL) == -1)
    {        
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }
    printf("Client get: %s\n", message_in);

    char *message_out = "Hello!";
    
    if (mq_send(mq_c2s, message_out, strlen(message_out), 9) == -1)
    {        
        perror("mq_send");
        exit(EXIT_FAILURE);
    }
    printf("Client send: %s\n", message_out);

    exit(EXIT_SUCCESS);
}