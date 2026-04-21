#include <sys/socket.h>
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


int listen_server()
{
    
    /*
    1) Создаем сокет
    2) Запускаем беск. цикл
    3) Ждем запрос клиента
    4) Создаем уникальный ендпоинт и отправляем его клиенту, клиент пытается подключиться
    5) Форкаем процесс
    6) Процесс обслуживает клиента и завершается 
    */

    struct sockaddr_in addr_serv, addr_client;
    int sockfd, clientfd, ret, port_service = 10000;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;                             
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(PORTLISTEN);

    ret = bind(sockfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv));     // Привязываем адрес к сокету
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN); // Для предотвращения зомби процессов 
    len = sizeof(struct sockaddr_in);
    while (1)
    {
        printf("Listen server wait client\n");
        clientfd = accept(sockfd, (struct sockaddr*)&addr_client, &len);
        printf("Listen server get client\n");

        pid_t pd = fork();

        if (pd == -1)
        {
            perror("fork");
            continue;
        }
        else if (pd == 0)
        {
            printf("Service server start service client\n");
            service_server(port_service, clientfd);
        }
        else
        {
            port_service++;
            if (port_service > 65536 - 1) port_service = 10000;
            close(clientfd);
            continue;
        }
    }
}

int service_server(int port, int clientfd_listen)
{
    struct sockaddr_in addr_serv, addr_client;
    struct tm *t;
    int sockfd, clientfd, ret;
    char buf[BUFF_SIZE];
    socklen_t len;
    time_t currentTime;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;                             
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(port);
    
    ret = bind(sockfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv));  
 
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Отправка обслуживающего порта клиенту через дескриптор созданным родителем
    if (send(clientfd_listen, &port, sizeof(int), 0) != sizeof(int))
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("Service server send port %d client\n", port);

    close(clientfd_listen);

    len = sizeof(struct sockaddr_in);

    clientfd = accept(sockfd, (struct sockaddr*)&addr_client, &len);

    /* Отправляем сообщение клиенту */
    currentTime = time(NULL);
    t = localtime(&currentTime);
   
    if (send(clientfd, t, sizeof(struct tm), 0) != sizeof(struct tm))
    {
        perror("send");
        exit(EXIT_FAILURE);
    }

    char time_str[BUFF_SIZE];
    
    strftime(time_str, BUFF_SIZE, "%d-%m-%Y %H:%M:%S", t);
    printf("Service server %d send message: %s\n", getpid(), time_str);    

    close(clientfd);
    close(sockfd);
    exit(EXIT_SUCCESS);
}

int main()
{
    listen_server();
    exit(EXIT_SUCCESS);
}