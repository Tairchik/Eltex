#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "shared_m.h"

int main()
{
    int shm_fd = shm_open(PATHNAME, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        if (errno == ENOENT)
            fprintf(stderr, "Server is not running\n");
        else
            perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, SIZEBUF) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    char *ptr = mmap(NULL, SIZEBUF, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sem_t *semaphore1 = sem_open(SEMNAME1, O_CREAT, 0666, 0);
    if (semaphore1 == SEM_FAILED)
    {
        perror("sem_open semaphore1");
        exit(EXIT_FAILURE);
    }
    sem_t *semaphore2 = sem_open(SEMNAME2, O_CREAT, 0666, 0);
    if (semaphore2 == SEM_FAILED)
    {
        sem_close(semaphore2);
        perror("sem_open semaphore2");
        exit(EXIT_FAILURE);
    }
    
    sem_wait(semaphore1);
    printf("Client get: %s\n", ptr);

    strcpy(ptr, "Hello!");
    printf("Client send: %s\n", ptr);

    sem_post(semaphore2);

    sem_close(semaphore1);
    sem_close(semaphore2);
    exit(EXIT_SUCCESS);
}