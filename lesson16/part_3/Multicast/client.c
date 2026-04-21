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
    struct sockaddr_in addr_serv;
    char buf[BUFF_SIZE];
    int sockfd, ret, flag = 1;
    struct ip_mreqn mreqn;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Создаем сокет, как у сервера
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&mreqn, 0, sizeof(struct ip_mreqn));

    mreqn.imr_multiaddr.s_addr = inet_addr("224.0.0.7");
    mreqn.imr_address.s_addr = htonl(INADDR_ANY);
    mreqn.imr_ifindex = 0;

    setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn));

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_serv.sin_port = htons(PORT);

    ret = bind(sockfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv));

    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Получаем сообщение от сервера */

    for (int i = 0; i < 10; i++)
    {
        if (recv(sockfd, buf, BUFF_SIZE, 0) == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        printf("Client receive message: %s\n", buf);
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}