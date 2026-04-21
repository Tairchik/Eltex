#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "server.h"


int main()
{
    struct sockaddr_in addr_serv; 
    struct tm t;
    int port_service;
    int sockfd, ret; 
    char time_str[BUFF_SIZE];
    socklen_t len;
    
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
    
    ret = connect(sockfd, (struct sockaddr *) &addr_serv, sizeof(addr_serv));
    if (ret == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    if (send(sockfd, "connect\0", BUFF_SIZE, 0) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }

    /* Получаем сообщение от сервера */
    if (recv(sockfd, &t, sizeof(struct tm), 0) != sizeof(struct tm))
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    strftime(time_str, BUFF_SIZE, "%d-%m-%Y %H:%M:%S", &t);
    printf("Client receive message: %s\n", time_str);

    close(sockfd);
    exit(EXIT_SUCCESS);
}