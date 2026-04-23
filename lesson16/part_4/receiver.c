#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024

int main()
{
    struct sockaddr_in server, client;
    struct in_addr src_addr, dst_addr;
    struct iphdr *ip; 
    struct udphdr *udp;
    socklen_t len;
    char message[BUFSIZE];
    int res, raw_sock, ip_header_len;

    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_sock == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = 0;
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    res = bind(raw_sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in));

    if (res == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        len = sizeof(struct sockaddr_in);
            
        if (recvfrom(raw_sock, message, BUFSIZE, 0, (struct sockaddr *) &client, &len) == -1)
        {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
    
        ip = (struct iphdr *) message;
        src_addr.s_addr = ip->saddr;
        dst_addr.s_addr = ip->daddr;

        udp = (struct udphdr *) &message[ip->ihl * 4];

        printf("-------IP-HEADER--------\n");
        printf("Version: %u\n", ip->version);
        printf("Header Length: %u bytes\n", ip->ihl * 4);
        printf("TTL: %u\n", ip->ttl);
        printf("Protocol: %u\n", ip->protocol);
        printf("Source IP: %s\n", inet_ntoa(src_addr));
        printf("Dest IP: %s\n", inet_ntoa(dst_addr));
        printf("-------UDP-HEADER--------\n");
        printf("Source Port: %u\n", ntohs(udp->source));
        printf("Dest Port: %u\n", ntohs(udp->dest));
    }

    close(raw_sock);
    exit(EXIT_SUCCESS);
}