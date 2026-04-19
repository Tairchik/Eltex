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
    struct sockaddr_un addr_serv; 
    char buf[BUFF_SIZE];
    int sockfd, ret; 
    socklen_t len;
    
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);                                     // Создаем сокет, как у сервера
    if (sockfd == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_un));
    
    addr_serv.sun_family = AF_LOCAL;                                              // Указываем локальный домен сервера
    strncpy(addr_serv.sun_path, SOCKET_NAME, sizeof(addr_serv.sun_path) - 1);     // Указываем условный адрес (путь)

    ret = connect(sockfd, (struct sockaddr *) &addr_serv, sizeof(struct sockaddr_un));
    if (ret == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* Получаем сообщение от сервера */
    if (recv(sockfd, buf, BUFF_SIZE, 0) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    
    printf("Client recive message: %s\n", buf);


    /* Отправляем сообщение серверу */
    strncpy(buf, "Hi!", BUFF_SIZE - 1);
    if (send(sockfd, buf, BUFF_SIZE, 0) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("Client send message: %s\n", buf);


    close(sockfd);
    exit(EXIT_SUCCESS);
}