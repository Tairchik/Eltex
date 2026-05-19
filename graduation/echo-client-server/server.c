#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "server.h"
#include "listClient.h"

int main()
{
    struct udphdr udp, *udp_rcv;
    struct iphdr *ip_rcv;
    struct sockaddr_in addr_serv, addr_client;
    struct listClient *list;
    struct client client, *client_ptr; 
    char message[BUFSIZE], *data, *message_send, str_count[20];
    socklen_t len;
    int sockfd, ret, count = 1;

    list = createList();
    if (!list)
    {
        perror("createList");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(SERVERPORT);
    if (inet_aton(SERVERIP, &addr_serv.sin_addr) == 0)
    {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }

    ret = bind(sockfd, (struct sockaddr *) &addr_serv, sizeof(addr_serv));
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }


    while (1)
    {
        len = sizeof(struct sockaddr_in);
        if (recvfrom(sockfd, message, BUFSIZE, 0, (struct sockaddr*) &addr_client, &len) == -1)
        {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        ip_rcv = (struct iphdr *) &message;
        udp_rcv = (struct udphdr *) &message[ip_rcv->ihl * 4];
        data = &message[ip_rcv->ihl * 4 + sizeof(struct udphdr)];

        if (udp_rcv->dest != htons(SERVERPORT)) 
            // Пакет не наш
            continue; 

        printf("Server receive message: %s\n", data);
        
        client.ip = ip_rcv->saddr;
        client.port = udp_rcv->source;
        client.count = 1;

        client_ptr = addClient(list, client);
        
        // Изменяем строку
        sprintf(str_count, " %d", client_ptr->count);
        strncat(data, str_count, sizeof(data));
        
        printf("New string: %s.\n", data);
        
        udp.check = 0;
        udp.dest = udp_rcv->source;
        udp.source = htons(SERVERPORT);
        udp.len = htons(8 + strlen(data) + 1);

        message_send = malloc(strlen(data) + 1 + sizeof(struct udphdr));

        if (message_send == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        
        memcpy(message_send, &udp, sizeof(struct udphdr));
        memcpy(message_send + sizeof(struct udphdr), data, strlen(data) + 1);

        len = sizeof(struct sockaddr_in);
        if (sendto(sockfd, message_send, strlen(data) + sizeof(struct udphdr) + 1, 0, (struct sockaddr *) &addr_client, len) == -1)
        {
            free(message_send);    
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        printf("Server send message: %s\n", &message_send[sizeof(struct udphdr)]);
        free(message_send);            
        client_ptr->count++;
    }

    freeListClient(list);
    exit(EXIT_SUCCESS);
}