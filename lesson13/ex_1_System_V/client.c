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
    
    int msqid = msgget(key, 0);
    if (msqid == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    msgbuffer message_in;
    if (msgrcv(msqid, &message_in, MSGLEN, 10, 0) == -1)
    {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    printf("Client get: %s\n", message_in.data);

    msgbuffer message_out;
    message_out.type = 9;
    strcpy(message_out.data, "Hello!\0");
    if (msgsnd(msqid, &message_out, MSGLEN, IPC_NOWAIT) == - 1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);   
    }

    printf("Client send: %s\n", message_out.data);
    exit(EXIT_SUCCESS);
}