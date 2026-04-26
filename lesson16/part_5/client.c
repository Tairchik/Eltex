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
    struct sockaddr_in addr_serv; 
    struct iphdr *ip; 
    char *message_send, *message = "Hi!";
    char buf_recv[BUFSIZE];
    int sockfd, ret, src_port = 8888, dest_port = 7777;
    socklen_t len;
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;     
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(dest_port);
    
    // Формируем udp заголовок
    udp.source = htons(src_port);
    udp.dest = htons(dest_port);
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
    memcpy(message_send + sizeof(struct udphdr), message, strlen(message));
    
    // Отправляем сообщение
    len = sizeof(struct sockaddr_in);
    if (sendto(sockfd, message_send, strlen(message) + sizeof(struct udphdr), 0, (struct sockaddr *) &addr_serv, len) == -1)
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
        if (recvfrom(sockfd, buf_recv, BUFSIZE, 0, (struct sockaddr*) &addr_serv, &len) == -1)
        {
            free(message_send);    
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        // Проверяем, что это наш пакет
        ip = (struct iphdr *) buf_recv;
        ptr_udp = (struct udphdr *) &buf_recv[ip->ihl * 4];
        
        // Проверяем наш ли это пакет
        if (ptr_udp->source != htons(dest_port) || ptr_udp->dest != htons(src_port)) 
            // Пакет не наш
            continue; 

        // Извлекаем сообщение
        char *recv_msf = (char *) &buf_recv[ip->ihl * 4 + sizeof(struct udphdr)];
        printf("Client receive message: %s\n", recv_msf);
        break;
    }
    
    free(message_send);    
    exit(EXIT_FAILURE);
}