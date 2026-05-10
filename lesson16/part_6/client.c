#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 20 + 8 + 128

int main()
{
    struct udphdr udp, *ptr_udp;
    struct iphdr ip, *ip_rcv;
    struct sockaddr_in addr_serv;
    char *message_send, *message = "Hi!";
    char buf_recv[BUFSIZE];
    int sockfd, ret, src_port = 8888, dest_port = 7777, flag = 1;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag));

    // Формируем ip заголовок
    ip.ihl = 5;
    ip.version = 4;
    ip.tos = 0;
    ip.tot_len = 0;
    ip.id = 0;
    ip.frag_off = 0;
    ip.ttl = 64;
    ip.protocol = IPPROTO_UDP;
    ip.check = 0;
    ip.saddr = 0;
    ip.daddr = htonl(INADDR_LOOPBACK);

    // Формируем udp заголовок
    udp.source = htons(src_port);
    udp.dest = htons(dest_port);
    udp.check = 0;
    udp.len = htons(8 + strlen(message));

    memset(&addr_serv, 0, sizeof(addr_serv));

    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr_serv.sin_port = htons(dest_port);

    // Соединяем ip, udp и payload для отправки
    message_send = malloc(strlen(message) + sizeof(struct udphdr) + sizeof(struct iphdr));
    if (message_send == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(message_send, &ip, sizeof(struct iphdr));
    memcpy(message_send + sizeof(struct iphdr), &udp, sizeof(struct udphdr));
    memcpy(message_send + sizeof(struct iphdr) + sizeof(struct udphdr), message, strlen(message));

    // Отправляем сообщение
    len = sizeof(struct sockaddr_in);
    if (sendto(sockfd, message_send, strlen(message) + sizeof(struct udphdr) + sizeof(struct iphdr), 0, (struct sockaddr *)&addr_serv, len) == -1)
    {
        free(message_send);
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    printf("Client send message: %s\n", &message_send[sizeof(struct udphdr) + sizeof(struct iphdr)]);

    // Принимаем сообщение
    while (1)
    {
        len = sizeof(struct sockaddr_in);
        if (recvfrom(sockfd, buf_recv, BUFSIZE, 0, (struct sockaddr *)&addr_serv, &len) == -1)
        {
            free(message_send);
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        ip_rcv = (struct iphdr *)buf_recv;
        ptr_udp = (struct udphdr *)&buf_recv[ip_rcv->ihl * 4];

        // Проверяем наш ли это пакет
        if (ptr_udp->source != htons(dest_port) || ptr_udp->dest != htons(src_port))
            // Пакет не наш
            continue;

        // Извлекаем сообщение
        char *recv_msf = (char *)&buf_recv[ip_rcv->ihl * 4 + sizeof(struct udphdr)];
        printf("Client receive message: %s\n", recv_msf);
        break;
    }

    free(message_send);
    exit(EXIT_FAILURE);
}