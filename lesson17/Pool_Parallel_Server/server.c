#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "server.h"

#define POOLSIZE 3
#define PATHNAME "/server.h"

sem_t *shm_ptr;

int listen_server()
{
    /*
    1) Создаем слушающий сокет
    2) Создаем n семафоров (ipc) для взаимодействия слушающего сервера и обслуживающего
    3) Создаем n обслуживающих сервисов
    4) Запускаем бесконечный цикл
    5) Ждем запрос клиента
    6) Выбираем первый свободный сервер
    7) Отправляем порт клиенту, клиент пытается подключится
    8) Процесс обслуживает клиента и завершается
    */

    // (1)
    struct sockaddr_in addr_serv, addr_client;
    int sockfd, clientfd, ret, port_service = 10000;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(PORTLISTEN);

    ret = bind(sockfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)); // Привязываем адрес к сокету
    if (ret == -1)
    {
        perror("Listen server bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN); // Для предотвращения зомби процессов

    // (2) - (3)
    shm_unlink(PATHNAME);
    int shm_fd = shm_open(PATHNAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, POOLSIZE * sizeof(sem_t)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shm_ptr = mmap(NULL, POOLSIZE * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == (void *)-1)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < POOLSIZE; i++)
    {
        if (sem_init(&shm_ptr[i], 1, 1) == -1)
        {
            perror("sem_init");
            exit(EXIT_FAILURE);
        }
    }

    int temp_port = port_service;

    for (int i = 0; i < POOLSIZE; i++)
    {
        pid_t pd = fork();
        if (pd == 0)
        {
            close(sockfd);
            service_server(temp_port, &shm_ptr[i]);
        }
        else if (pd == -1)
        {
            perror("fork");
            continue;
        }
        else
        {
            temp_port++;
        }
    }

    // (4)
    len = sizeof(struct sockaddr_in);
    while (1)
    {
        printf("Listen server wait client\n");
        clientfd = accept(sockfd, (struct sockaddr *)&addr_client, &len);
        printf("Listen server get client\n");

        // Выбираем обслуживающий сервер
        for (int i = 0; i < POOLSIZE; i++)
        {
            if (sem_trywait(&shm_ptr[i]))
            {
                if (errno == EAGAIN)
                {
                    if (i + 1 == POOLSIZE)
                    {
                        i = -1; // Ищем заново
                        usleep(10000);
                    }
                    continue; // Ищем другой, свободный канал
                }
                perror("sem_trywait");
                exit(EXIT_FAILURE);
            }

            // Отправка обслуживающего порта клиенту
            int tmp = port_service + i;
            if (send(clientfd, &tmp, sizeof(int), 0) != sizeof(int))
            {
                perror("send");
                exit(EXIT_FAILURE);
            }
            printf("Listen server send port %d to client\n", tmp);
            close(clientfd);

            break;
        }
    }
}

int service_server(int port, sem_t *sem)
{
    struct sockaddr_in addr_serv, addr_client;
    int sockfd, clientfd, ret, read_fl;
    char buf[BUFF_SIZE];
    socklen_t len;

    struct timeval timeout;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_serv, 0, sizeof(struct sockaddr_in));

    addr_serv.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr_serv.sin_addr);
    addr_serv.sin_port = htons(port);

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt timeout");
        exit(EXIT_FAILURE);
    }

    ret = bind(sockfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv));
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

    while (1)
    {
        len = sizeof(struct sockaddr_in);

        // Получаем новый дескриптор клиента
        clientfd = accept(sockfd, (struct sockaddr *)&addr_client, &len);

        if (clientfd < 0)
        {  
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                int sem_val;
                sem_getvalue(sem, &sem_val);

                // Если семафор заблокировали, но клиент так и не пришел
                if (sem_val == 0)
                {
                    printf("Service server %d: Timeout reached, client didn't connect. Releasing semaphore...\n", getpid());
                    sem_post(sem);
                }
                continue; 
            }
            else
            {
                perror("accept error");
                exit(EXIT_FAILURE);
            }
        }
        /* Отправляем сообщение клиенту */
        strncpy(buf, "Hello!", BUFF_SIZE - 1);
        if (send(clientfd, buf, strlen(buf) + 1, 0) != strlen(buf) + 1)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        printf("Service server %d send message: %s\n", getpid(), buf);

        /* Получаем сообщение от клиента */
        if (recv(clientfd, buf, BUFF_SIZE, 0) == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        printf("Service server %d receive message: %s\n", getpid(), buf);

        close(clientfd);

        // Уведомляю о том, что освободились
        int sem_val;
        
        sem_getvalue(sem, &sem_val);
        if (sem_val == 1) continue;

        if (sem_post(sem) == -1)
        {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    listen_server();
    exit(EXIT_SUCCESS);
}