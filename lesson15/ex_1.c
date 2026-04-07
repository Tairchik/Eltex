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
Реализовать программу, которая переопределяет диспозицию сигнал
SIGUSR1 с помощью вызова sigaction(), после чего уходит в бесконечный
цикл ожидания. В функции диспозиции сигнала на экран выводится
сообщение о получении сигнала. Также необходимо реализовать вторую
программу, которая будет посылать первой программе сигнал SIGUSR1
через вызов kill(). Дополнительно научится посылать сигнал с помощью
утилиты kill.
*/

sem_t *semaphore1;
pid_t *ptr;
int shm_fd;

void sig_handler(int sig_num, siginfo_t *info, void *args)
{
    if (sig_num == SIGUSR1) {
    printf("Get signal SIGUSR1\n");
    }
    else
    {
        sem_close(semaphore1);
        sem_unlink(SEMNAME);

        munmap(ptr, sizeof(pid_t));
        close(shm_fd);
        shm_unlink(PATHNAME);

        exit(EXIT_SUCCESS);
    }
}

int main()
{
    struct sigaction handler;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGINT);

    handler.sa_sigaction = sig_handler;
    handler.sa_mask = set;

    if (sigaction(SIGUSR1, &handler, NULL) < 0)
    {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGINT, &handler, NULL) < 0)
    {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }

    // Создаем разделяемую пымять, чтобы отправить туда свой pid
    shm_fd = shm_open(PATHNAME, O_CREAT | O_RDWR, 0666);
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
    
    ptr = mmap(NULL, sizeof(pid_t),PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    semaphore1 = sem_open(SEMNAME, O_CREAT, 0666, 0);
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