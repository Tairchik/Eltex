#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "queue.h"

#define CLIENTS 100
#define MAX_MSG 1000

typedef struct
{
    int pid;
    int qid;
    char name[MAX_NAME];
} Client;

Client clients[CLIENTS];
Message msg_buff[MAX_MSG] = {0};
int msg_count = 0;
int client_count = 0;

int create_queue(const char *path, int id)
{
    key_t key = ftok(path, id);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int qid = msgget(key, 0666 | IPC_CREAT);
    if (qid == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    return qid;
}

void send_all_messages_to_client(Client client)
{
    int i = 0;
    while (i < msg_count)
    {
        msgsnd(client.qid, &msg_buff[i], LEN_MSG, 0);
        i++;
    }
}

void broadcast(Message *msg)
{
    for (int i = 0; i < client_count; i++)
    {
        msgsnd(clients[i].qid, msg, LEN_MSG, 0);
    }
}

int find_client(int pid)
{
    for (int i = 0; i < client_count; i++)
        if (clients[i].pid == pid)
            return i;
    return -1;
}

int main()
{
    int qid_login = create_queue(PATHNAME_LOGIN, PATHNAME_LOGIN_ID);

    printf("Server started\n");

    while (1)
    {
        Message msg;

        if (msgrcv(qid_login, &msg, LEN_MSG, 0, 0) == -1)
        {
            perror("msgrcv");
            continue;
        }

        // LOGIN
        if (msg.type == MSG_LOGIN)
        {
            if (find_client(msg.pid_client) != -1)
                continue;

            int qid_client = msgget(msg.pid_client, 0666 | IPC_CREAT);

            clients[client_count].pid = msg.pid_client;
            clients[client_count].qid = qid_client;
            strcpy(clients[client_count].name, msg.name);
            client_count++;

            printf("Client joined: %s\n", msg.name);
            send_all_messages_to_client(clients[client_count - 1]);

            Message msg_client_join;
            strcpy(msg_client_join.name, msg.name);
            msg_client_join.pid_client = msg.pid_client;
            msg_client_join.text[0] = '\0';
            msg_client_join.type = MSG_SERVER;
            msg_buff[msg_count] = msg_client_join;
            msg_count++;
            broadcast(&msg_client_join);
        }

        // CHAT MESSAGE
        else if (msg.type == MSG_TEXT)
        {
            msg_buff[msg_count] = msg;
            msg_count++;
            broadcast(&msg);
        }
    }

    return 0;
}