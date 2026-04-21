#include <sys/socket.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "server.h"

#define POOLSIZE 3
#define PATHNAME "/server.h"
#define QUEUEPATH "/mq"

int listen_server()
{
    /*
    1) Создаем слушающий сервер
    2) Создаем очередь
    3) Создаем пул обслуживающих серверов
    4) Принимаем запрос клиента
    5) Отправляем endpoint в очередь
    6) Один из обслуживающий серверов принимает и выполняет заявку
    */

    // (1)
    struct sockaddr_in addr_serv, addr_client;
    int sockfd, clientfd, ret, port_service = 10000;
    socklen_t len;
    char buf[BUFF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(PORTLISTEN);

    ret = bind(sockfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)); // Привязываем адрес к сокету
    if (ret == -1)
    {
        perror("Listen server bind");
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN);
    
    // (2)
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct sockaddr_in);

    mq_unlink(QUEUEPATH);

    mqd_t queue = mq_open(QUEUEPATH, O_CREAT | O_WRONLY, 0666, &attr);
    
    if (queue == -1) 
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    //(3)
    for (int i = 0; i < POOLSIZE; i++)
    {
        pid_t pd = fork();
        if (pd == 0)
        {
            service_server(port_service, sockfd);
        }
        else if (pd == -1)
        {
            perror("fork");
            continue;
        }
    }

    // (4)
    len = sizeof(struct sockaddr_in);
    while (1)
    {
        printf("Listen server wait client\n");
        if (recvfrom(sockfd, buf, BUFF_SIZE, 0, 
        (struct sockaddr *) &addr_client, &len) == -1)
        {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("Listen server get client\n");

        if (mq_send(queue, (char *)&addr_client, sizeof(struct sockaddr_in), 0) == -1)
        {
            perror("mq_send");
        }
    }
}

int service_server(int port, int sockfd)
{
    struct tm *t;
    time_t currentTime;
    socklen_t len;
    char time_str[BUFF_SIZE];
    struct sockaddr_in addr_serv, addr_client;

    mqd_t queue = mq_open(QUEUEPATH, O_RDONLY);

    while (1)
    {
        if (mq_receive(queue, (char *)&addr_client, sizeof(struct sockaddr_in), NULL) == -1)
        {        
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
        
        /* Отправляем сообщение клиенту */
        currentTime = time(NULL);
        t = localtime(&currentTime);
        len = sizeof(struct sockaddr_in);
        if (sendto(sockfd, t, sizeof(struct tm), 0,  (struct sockaddr *) &addr_client, len) != sizeof(struct tm))
        {
            perror("send");
            exit(EXIT_FAILURE);
        }

        char time_str[BUFF_SIZE];

        strftime(time_str, BUFF_SIZE, "%d-%m-%Y %H:%M:%S", t);
        printf("Service server %d send message: %s\n", getpid(), time_str); 
    }
}

int main()
{
    listen_server();
    exit(EXIT_SUCCESS);
}