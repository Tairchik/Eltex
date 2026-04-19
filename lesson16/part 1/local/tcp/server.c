#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>

/* В реальных приложениях указывают абсолютный путь см. Linix API
исчерпавающее издание гл. 53.1 "Адреса сокетов в домене UNIX: struct sockaddr_un" */
#define SOCKET_NAME "/tmp/tcp_socket"
#define BUFF_SIZE 64

int main()
{
    struct sockaddr_un addr_serv, addr_client;
    char buf[BUFF_SIZE];
    int sockfd, clientfd, ret;
    socklen_t len;

    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0); // Создаем сокет
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (remove(SOCKET_NAME) == -1 && errno != ENOENT)
    {
        perror("remove");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_un));

    addr_serv.sun_family = AF_LOCAL;                                          // Указываем локальный домен
    strncpy(addr_serv.sun_path, SOCKET_NAME, sizeof(addr_serv.sun_path) - 1); // Указываем условный адрес (путь)

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

    len = sizeof(struct sockaddr_un);
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
    printf("Server recive message: %s\n", buf);

    close(sockfd);
    remove(SOCKET_NAME);

    exit(EXIT_SUCCESS);
}