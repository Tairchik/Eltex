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
#define PORT 8888

int main()
{
    struct sockaddr_in addr_serv, addr_client;
    char buf[BUFF_SIZE];
    int sockfd, clientfd, ret;
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
    addr_serv.sin_port = htons(PORT);

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

    len = sizeof(struct sockaddr_in);
    clientfd = accept(sockfd, (struct sockaddr*)&addr_client, &len);

    /* Отправляем сообщение клиенту */
    strncpy(buf, "Hello!", BUFF_SIZE);
    if (send(clientfd, buf, BUFF_SIZE, 0) != BUFF_SIZE)
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    printf("Server send message: %s\n", buf);

    /* Получаем сообщение от клиента */
    if (recv(clientfd, buf, BUFF_SIZE, 0) == -1)
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    printf("Server receive message: %s\n", buf);

    close(sockfd);

    exit(EXIT_SUCCESS);
}