#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "server.h"

int listen_server()
{
    /*
    1) Создаем сокет TCP и UDP
    2) Создаем epoll\poll
    3) Добавляем сокеты в epoll
    4) Ждем завершение блокировки wait
    5) Обслуживаем клиента в зависимости от протокола TCP или UDP
    */

    struct sockaddr_in addr_serv, addr_client;
    int sockfd_tcp, sockfd_udp, clientfd, ret, epfd, port_service = 10000;
    socklen_t len;
    struct epoll_event ev_tcp, ev_udp, ev_list[2];
    time_t currentTime;
    struct tm *t;

    sockfd_tcp = socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет TCP
    if (sockfd_tcp == -1)
    {
        perror("stream socket");
        exit(EXIT_FAILURE);
    }

    sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0); // Создаем сокет UDP
    if (sockfd_udp == -1)
    {
        perror("dgram socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(PORTLISTEN);

    ret = bind(sockfd_tcp, (struct sockaddr *)&addr_serv, sizeof(addr_serv));
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    ret = bind(sockfd_udp, (struct sockaddr *)&addr_serv, sizeof(addr_serv));
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd_tcp, 5) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // (2)
    epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    ev_tcp.data.fd = sockfd_tcp;
    ev_tcp.events = EPOLLIN;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd_tcp, &ev_tcp) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    ev_udp.data.fd = sockfd_udp;
    ev_udp.events = EPOLLIN;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd_udp, &ev_udp) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    int ready;
    while (1)
    {
        ready = epoll_wait(epfd, ev_list, 2, NULL);
        if (ready == -1)
        {
            if (errno == EINTR)
                continue;
            /* Перезапускаем, если операция была прервана сигналом */
            else
            {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
        }

        /* Обрабатываем полученный список событий */
        for (int i = 0; i < ready; i++)
        {
            if (ev_list[i].data.fd == sockfd_tcp) // TCP
            {
                printf("Service tcp-client\n");

                len = sizeof(struct sockaddr_in);
                clientfd = accept(sockfd_tcp, (struct sockaddr *) &addr_client, &len);

                currentTime = time(NULL);
                t = localtime(&currentTime);
            
                if (send(clientfd, t, sizeof(struct tm), 0) != sizeof(struct tm))
                {
                    perror("send");
                    exit(EXIT_FAILURE);
                }

                close(clientfd);
            }
            else                                 // UDP
            {
                printf("Service udp-client\n");
                len = sizeof(struct sockaddr_in);

                if (recvfrom(sockfd_udp, NULL, 0, 0, 
                (struct sockaddr *) &addr_client, &len) == -1)
                {
                    perror("recvfrom");
                    exit(EXIT_FAILURE);
                }

                currentTime = time(NULL);
                t = localtime(&currentTime);
                if (sendto(sockfd_udp, t, sizeof(struct tm), 0,  (struct sockaddr *) &addr_client, len) != sizeof(struct tm))
                {
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

int main()
{
    listen_server();
    exit(EXIT_SUCCESS);
}