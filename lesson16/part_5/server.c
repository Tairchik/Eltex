#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 64
#define PORT 7777

int main()
{
    struct sockaddr_in addr_serv, addr_client; 
    char buf[BUFF_SIZE];
    int sockfd, ret; 
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);                                     // Создаем сокет
    if (sockfd == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));
    
    addr_serv.sin_family = AF_INET;                                         
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(PORT);

    ret = bind(sockfd, (struct sockaddr *) &addr_serv, sizeof(addr_serv));        // Привязываем адрес к сокету
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    len = sizeof(struct sockaddr_in);
    /* Получаем сообщение от клиента */
    if (recvfrom(sockfd, buf, BUFF_SIZE, 0, 
        (struct sockaddr *) &addr_client, &len) == -1)
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    
    printf("Server receive message: %s\n", buf);

    /* Отправляем сообщение клиенту */
    strncpy(buf, "Hello!", BUFF_SIZE);
    if (sendto(sockfd, buf, BUFF_SIZE, 0, 
        (struct sockaddr *) &addr_client, len) != BUFF_SIZE)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    
    printf("Server send message: %s\n", buf);

    close(sockfd);
    exit(EXIT_SUCCESS);
}