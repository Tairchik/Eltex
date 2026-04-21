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
    struct sockaddr_in addr_send;
    char buf[BUFF_SIZE];
    int sockfd, ret;
    socklen_t len;
    int flag = 1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_send, 0, sizeof(struct sockaddr_in));

    addr_send.sin_family = AF_INET;
    inet_aton("224.0.0.7", &addr_send.sin_addr);
    addr_send.sin_port = htons(PORT);

    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    len = sizeof(struct sockaddr_in);

    /* Отправляем сообщение клиентам */
    printf("Server send messages.\n");
    for (int i = 0; i < 10; i++)
    {
        strncpy(buf, "Hello!", BUFF_SIZE);
        if (sendto(sockfd, buf, BUFF_SIZE, 0,
                   (struct sockaddr *)&addr_send, len) != BUFF_SIZE)
        {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}