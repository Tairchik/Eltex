#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "server.h"

#define CLIENTPORT 5432
#define CLEINTIP "127.0.0.1"

int main()
{
    struct udphdr udp, *udp_rcv;
    struct iphdr *ip_rcv;
    struct sockaddr_in addr_serv, addr_client;
    char message[MSGSIZE], *data, *message_send, message_rcv[BUFSIZE];
    socklen_t len;
    int sockfd, ret;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(SERVERPORT);
    if (inet_aton(SERVERIP, &addr_serv.sin_addr) == 0)
    {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Введите строку:\n");
        fgets(message, sizeof(message), stdin);

        message[strcspn(message, "\r\n")] = '\0';
        // Формируем udp заголовок
        udp.source = htons(CLIENTPORT);
        udp.dest = htons(SERVERPORT);
        udp.check = 0;
        udp.len = htons(8 + strlen(message));

        // Соединяем udp и message для отправки
        message_send = malloc(strlen(message) + sizeof(struct udphdr));
        if (message_send == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        memcpy(message_send, &udp, sizeof(struct udphdr));
        memcpy(message_send + sizeof(struct udphdr), message, strlen(message) + 1);

        // Отправляем сообщение
        len = sizeof(struct sockaddr_in);
        if (sendto(sockfd, message_send, strlen(message) + 1 + sizeof(struct udphdr), 0, (struct sockaddr *)&addr_serv, len) == -1)
        {
            free(message_send);
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        printf("Client send message: %s\n", &message_send[sizeof(struct udphdr)]);

        // Принимаем сообщение
        while (1)
        {
            len = sizeof(struct sockaddr_in);
            if (recvfrom(sockfd, message_rcv, BUFSIZE, 0, (struct sockaddr *)&addr_serv, &len) == -1)
            {
                free(message_send);
                perror("recvfrom");
                exit(EXIT_FAILURE);
            }

            ip_rcv = (struct iphdr *)&message_rcv;
            udp_rcv = (struct udphdr *)&message_rcv[ip_rcv->ihl * 4];
            data = &message_rcv[ip_rcv->ihl * 4 + sizeof(struct udphdr)];

            if (udp_rcv->dest != htons(CLIENTPORT))
                // Пакет не наш
                continue;

            printf("Client receive message: %s\n", data);
            break;
        }

        free(message_send);
    }

    exit(EXIT_SUCCESS);
}