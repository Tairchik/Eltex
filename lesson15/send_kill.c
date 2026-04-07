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
Посылаем сигнал kill серверам
*/

int main()
{
    // Подключаемся к каналу, читаем и отправлеям сигнал
    int shm_fd = shm_open(PATHNAME, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        if (errno == ENOENT)
            fprintf(stderr, "Server is not running\n");
        else
            perror("shm_open");
        exit(EXIT_FAILURE);
    }

    pid_t *ptr = mmap(NULL, sizeof(pid_t), PROT_READ, MAP_SHARED, shm_fd, 0);
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
    
    sem_wait(semaphore1);
    kill(*ptr, SIGUSR1);
    sem_post(semaphore1);


    sem_close(semaphore1);
    close(shm_fd);

    exit(EXIT_SUCCESS);
}