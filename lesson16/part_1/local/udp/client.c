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
#define SOCKET_NAME "/tmp/udp_socket"
#define CLIENT_SOCKET_NAME "/tmp/udp_socket_client"
#define BUFF_SIZE 64

int main()
{
    struct sockaddr_un addr_serv, addr_client; 
    char buf[BUFF_SIZE];
    int sockfd, ret; 
    socklen_t len;
    
    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);                                     // Создаем сокет, как у сервера
    if (sockfd == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (remove(CLIENT_SOCKET_NAME) == -1 && errno != ENOENT)
    {
        perror("remove");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_un));
    
    addr_serv.sun_family = AF_LOCAL;                                              // Указываем локальный домен сервера
    strncpy(addr_serv.sun_path, SOCKET_NAME, sizeof(addr_serv.sun_path) - 1);     // Указываем условный адрес (путь)

    memset(&addr_client, 0, sizeof(addr_client));
    addr_client.sun_family = AF_LOCAL;
    strncpy(addr_client.sun_path, CLIENT_SOCKET_NAME,
            sizeof(addr_client.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&addr_client,
            sizeof(addr_client)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    ret = connect(sockfd, (struct sockaddr *) &addr_serv, sizeof(struct sockaddr_un));
    if (ret == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    len = sizeof(struct sockaddr_un);
    /* Отправляем сообщение серверу */
    strncpy(buf, "Hi!", BUFF_SIZE - 1);
    if (send(sockfd, buf, BUFF_SIZE, 0) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("Client send message: %s\n", buf);

    /* Получаем сообщение от сервера */
    if (recv(sockfd, buf, BUFF_SIZE, 0) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    
    printf("Client recive message: %s\n", buf);

    close(sockfd);
    remove(CLIENT_SOCKET_NAME);

    exit(EXIT_SUCCESS);
}