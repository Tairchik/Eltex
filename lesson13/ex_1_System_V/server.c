#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

int main()
{
    key_t key = ftok(PATHNAME, PROJECTID);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    msgbuffer message_out;
    strcpy(message_out.data, "Hi!\0");
    message_out.type = 10;
    
    if (msgsnd(msqid, &message_out, MSGLEN, IPC_NOWAIT) == -1) 
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("Server send: %s\n", message_out.data);

    msgbuffer message_in;
    if (msgrcv(msqid, &message_in, MSGLEN, 9, 0) == -1)
    {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Server get: %s\n", message_in.data);
    exit(EXIT_SUCCESS);
}