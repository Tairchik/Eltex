#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "shared_m.h"

int main()
{
    key_t key = ftok(PATHNAME, PROJECTID);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shm_id = shmget(key, SIZEBUF, IPC_CREAT | 0666);
    if (shm_id == -1)
    {
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
    int sem_id = semget(key_sem, 2, IPC_CREAT | 0666);

    union semun arg;
    unsigned short values[2] = {1, 1};
    arg.array = values;

    semctl(sem_id, 0, SETALL, arg);

    strcpy(shmp, "Hi!");
    printf("Server send: %s\n", shmp);

    semop(sem_id, &unlock_1, 1);
    
    semop(sem_id, lock_2, 2);
    printf("Server get: %s\n", shmp);
    semop(sem_id, &unlock_2, 1);

    if (shmctl(shm_id, IPC_RMID, NULL) == -1) 
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    if (semctl(sem_id, 0, IPC_RMID) == -1)
    {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}