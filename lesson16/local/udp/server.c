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
#define BUFF_SIZE 64

int main()
{
    struct sockaddr_un addr_serv, addr_client; 
    char buf[BUFF_SIZE];
    int sockfd, clientfd, ret; 
    socklen_t len;

    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);                                     // Создаем сокет
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
    
    addr_serv.sun_family = AF_LOCAL;                                              // Указываем локальный домен
    strncpy(addr_serv.sun_path, SOCKET_NAME, sizeof(addr_serv.sun_path) - 1);     // Указываем условный адрес (путь)

    ret = bind(sockfd, (struct sockaddr *) &addr_serv, sizeof(addr_serv));        // Привязываем адрес к сокету
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    len = sizeof(struct sockaddr_un);
    /* Получаем сообщение от клиента */
    if (recvfrom(sockfd, buf, BUFF_SIZE, 0, 
        (struct sockaddr *) &addr_client, &len) == -1)
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    
    printf("Server recive message: %s\n", buf);

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
    remove(SOCKET_NAME);

    exit(EXIT_SUCCESS);
}