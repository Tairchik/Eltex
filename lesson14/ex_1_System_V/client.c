#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h>
#include "shared_m.h"

int main()
{
    key_t key = ftok(PATHNAME, PROJECTID);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shm_id = shmget(key, SIZEBUF, 0);
    if (shm_id == -1)
    {
        if (errno == ENOENT)
            fprintf(stderr, "Server is not running\n");
        else
            perror("shmget");
        exit(EXIT_FAILURE);
    }

    char *shmp;
    shmp = shmat(shm_id, NULL, 0);
    if (shmp == (void*) -1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    key_t key_sem = ftok(PATHNAME, SEMID);
    int sem_id = semget(key_sem, 2, 0);
    if (sem_id == -1)
    {
        if (errno == ENOENT)
            fprintf(stderr, "Server is not running semaphores\n");
        else
            perror("semget");
        exit(EXIT_FAILURE);
    }
    
    struct sembuf lock_1[2] = {{0, 0, 0}, {0, 1, 0}}; 
    struct sembuf unlock_1 = {0, -1, 0}; 
    struct sembuf lock_2[2] = {{1, 0, 0}, {1, 1, 0}}; 
    struct sembuf unlock_2 = {1, -1, 0}; 

    semop(sem_id, lock_1, 2);
    printf("Client get: %s\n", shmp);

    strcpy(shmp, "Hello!");
    printf("Client send: %s\n", shmp);
    semop(sem_id, &unlock_2, 1);
    
    shmdt(shmp);
    exit(EXIT_SUCCESS);
}