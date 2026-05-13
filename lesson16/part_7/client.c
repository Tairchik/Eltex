#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 14 + 20 + 8 + 128
#define DSTMAC "00:15:5d:27:23:a2"
#define SRCMAC "00:24:7d:c8:3A:b1"
#define DSTIP "192.168.0.52"
#define SRCIP "192.168.0.110"
#define NAMEINTERFACE "eth0"
#define SRCPORT 8888
#define DSTPORT 7777

uint16_t check_sum(short *ip_hdr, int len_hdr)
{
    uint32_t csum = 0;
    for (int i = 0; i < len_hdr; i++)
    {
        csum += *ip_hdr;
        ip_hdr++;
    }

    while (csum >> 16)
    {
        csum = (csum & 0xFFFF) + (csum >> 16);
    }
    
    return ~csum;
}

int main()
{
    struct udphdr udp, *ptr_udp;
    struct iphdr ip, *ip_rcv;
    struct ethhdr eth, *eth_rcv;
    struct sockaddr_ll addr_serv;
    struct ether_addr *mac, src_mac_bin, dst_mac_bin;
    char *message_send, *message = "Hi!";
    char buf_recv[BUFSIZE];
    int sockfd, ret, flag = 1;
    ssize_t msg_size = 0;
    socklen_t len;

    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Формируем ethernet заголовок
    eth.h_proto = htons(ETH_P_IP);
    mac = ether_aton(DSTMAC);
    if (mac)
    {
        memcpy(eth.h_dest, mac->ether_addr_octet, 6);
    }

    mac = ether_aton(SRCMAC);
    if (mac)
    {
        memcpy(eth.h_source, mac->ether_addr_octet, 6);
    }

    // Формируем ip заголовок
    memset(&ip, 0, sizeof(ip));
    ip.ihl = 5;
    ip.version = 4;
    ip.tos = 0;
    ip.tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(message));;
    ip.id = 0;
    ip.frag_off = 0;
    ip.ttl = 64;
    ip.protocol = IPPROTO_UDP;
    ip.saddr = inet_addr(SRCIP);
    ip.daddr = inet_addr(DSTIP);
    ip.check = check_sum((short *)&ip, 10);

    // Формируем udp заголовок
    udp.source = htons(SRCPORT);
    udp.dest = htons(DSTPORT);
    udp.check = 0;
    udp.len = htons(8 + strlen(message));
    
    memset(&addr_serv, 0, sizeof(addr_serv));

    addr_serv.sll_family = AF_PACKET;
    addr_serv.sll_ifindex = if_nametoindex(NAMEINTERFACE);
    addr_serv.sll_halen = 6;

    mac = ether_aton(DSTMAC);
    if (mac)
    {
        memcpy(addr_serv.sll_addr, mac->ether_addr_octet, 6);
    }

    /* Альтернативный вариант:
    addr_serv.sll_addr[0] = 0x00;
    addr_serv.sll_addr[1] = 0x15;
    addr_serv.sll_addr[2] = 0x5d;
    addr_serv.sll_addr[3] = 0x27;
    addr_serv.sll_addr[4] = 0x23;
    addr_serv.sll_addr[5] = 0xa2;
    */

    // Соединяем ethernet, ip, udp и payload для отправки
    msg_size = strlen(message) + sizeof(struct udphdr) + sizeof(struct iphdr) + sizeof(struct ethhdr);
    message_send = malloc(msg_size);
    if (message_send == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(message_send, &eth, sizeof(struct ethhdr));
    memcpy(message_send + sizeof(struct ethhdr), &ip, sizeof(struct iphdr));
    memcpy(message_send + sizeof(struct ethhdr) + sizeof(struct iphdr), &udp, sizeof(struct udphdr));
    memcpy(message_send + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr), message, strlen(message));

    // Отправляем сообщение
    len = sizeof(struct sockaddr_ll);
    if (sendto(sockfd, message_send, msg_size, 0, (struct sockaddr *)&addr_serv, len) == -1)
    {
        free(message_send);
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    printf("Client send message: %s\n", &message_send[msg_size - strlen(message)]);

    // Принимаем сообщение
    while (1)
    {
        len = sizeof(struct sockaddr_ll);
        if (recvfrom(sockfd, buf_recv, BUFSIZE, 0, (struct sockaddr *)&addr_serv, &len) == -1)
        {
            free(message_send);
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        eth_rcv = (struct ethhdr *)buf_recv;
        ip_rcv = (struct iphdr *)(buf_recv + sizeof(struct ethhdr));
        ptr_udp = (struct udphdr *)((uint8_t *)ip_rcv + (ip_rcv->ihl * 4));
        
        // Проверяем наш ли это пакет
        memcpy(src_mac_bin.ether_addr_octet, ether_aton(SRCMAC)->ether_addr_octet, 6);
        memcpy(dst_mac_bin.ether_addr_octet, ether_aton(DSTMAC)->ether_addr_octet, 6);
        
        if (!(ptr_udp->source == htons(DSTPORT) && ptr_udp->dest == htons(SRCPORT) &&
            ip_rcv->daddr == inet_addr(SRCIP) && ip_rcv->saddr == inet_addr(DSTIP) &&
            memcmp(eth_rcv->h_dest, src_mac_bin.ether_addr_octet, 6) == 0 && memcmp(eth_rcv->h_source, dst_mac_bin.ether_addr_octet, 6) == 0))
            continue;

        // Извлекаем сообщение
        char *recv_msf = (char *)&buf_recv[ip_rcv->ihl * 4 + sizeof(struct udphdr) + sizeof(struct ethhdr)];
        printf("Client receive message: %s\n", recv_msf);
        break;
    }

    free(message_send);
    exit(EXIT_SUCCESS);
}