#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include "shared_m.h"

/*
Реализовать программу, которая будет блокировать сигнал SIGINT с
помощью вызова sigprocmask() и уходить в бесконечный цикл. Проверить
работу программы с помощью второй программы из задания 1 и утилиты
kill.
*/

int main()
{
    sigset_t set;
    int sig_num;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    if (sigprocmask(SIG_BLOCK, &set, NULL))
    {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    // Создаем разделяемую пымять, чтобы отправить туда свой pid
    int shm_fd = shm_open(PATHNAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        if (errno == ENOENT)
            fprintf(stderr, "Server is not running\n");
        else
            perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (shm_fd == -1) 
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(pid_t)) == -1) 
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    
    pid_t *ptr = mmap(NULL, sizeof(pid_t),PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sem_t *semaphore1 = sem_open(SEMNAME, O_CREAT, 0666, 0);
    if (semaphore1 == SEM_FAILED)
    {
        perror("sem_open semaphore1");
        exit(EXIT_FAILURE);
    }

    *ptr = getpid();
    sem_post(semaphore1);

    while (1)
    {
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}