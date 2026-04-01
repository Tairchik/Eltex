#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

int main()
{
    mq_unlink(SERVER_TO_CLIENT);
    mq_unlink(CLIENT_TO_SERVER);

    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSGLEN;

    mqd_t mq_s2c = mq_open(SERVER_TO_CLIENT, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq_s2c == -1) 
    {
        perror("SERVER_TO_CLIENT mq_open");
        exit(EXIT_FAILURE);
    }
    mqd_t mq_c2s = mq_open(CLIENT_TO_SERVER, O_CREAT | O_RDONLY, 0666, &attr);
    if (mq_c2s == -1) 
    {
        perror("CLIENT_TO_SERVER  mq_open");
        exit(EXIT_FAILURE);
    }
    char *message_out = "Hi!";
    if (mq_send(mq_s2c, message_out, strlen(message_out), 10) == -1)
    {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }
    printf("Server send: %s\n", message_out);

    char message_in[MSGLEN];
    
    if (mq_receive(mq_c2s, message_in, MSGLEN, NULL) == -1)
    {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }
    printf("Server get: %s\n", message_in);

    mq_unlink(SERVER_TO_CLIENT);
    mq_unlink(CLIENT_TO_SERVER);

    exit(EXIT_SUCCESS);
}