#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "client.h"


int safe_atoi(const char *num)
{
    int res = atoi(num);
    if (res <= 1023 || res > 65535)
    {
        fprintf(stderr, "Invalid port range\n");
        exit(EXIT_FAILURE);
    }
    return res;
}

int main(int argc, char *argv[])
{
    struct udphdr udp, *udp_rcv;
    struct iphdr *ip_rcv;
    struct sockaddr_in addr_serv, addr_client;
    char message[MSGSIZE], *data, *message_send, message_rcv[BUFSIZE];
    socklen_t len;
    int sockfd, ret;
    int portS, portC;
    char *ipS = NULL, *ipC = NULL;

    if (argc != 5 && argc != 3 || strcmp(argv[1], "--help") == 0)
    {
        fprintf(stderr, "Usage: %s [CLIENT ADDRESS] [CLIENT PORT] [SERVER ADDRESS] [SERVER PORT]\n\tor\n[CLIENT PORT] [SERVER PORT] to use loopback interface\n", argv[0]);
        fprintf(stderr, "If you want to exit the client, write \"exit\"\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 3)
    {
        ipC = "127.0.0.1";
        portC = safe_atoi(argv[1]);
        ipS = "127.0.0.1";
        portS = safe_atoi(argv[2]);
    }
    else if (argc == 5)
    {
        ipC = argv[1];
        portC = safe_atoi(argv[2]);
        ipS = argv[3];
        portS = safe_atoi(argv[4]);
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(portS);
    if (inet_aton(ipS, &addr_serv.sin_addr) == 0)
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
        udp.source = htons(portC);
        udp.dest = htons(portS);
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

        if (strcmp(message, "exit") == 0)
        {
            free(message_send);
            exit(EXIT_SUCCESS);
        }
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

            if (udp_rcv->dest != htons(portC))
                // Пакет не наш
                continue;

            printf("Client receive message: %s\n", data);
            break;
        }

        free(message_send);
    }

    exit(EXIT_SUCCESS);
}