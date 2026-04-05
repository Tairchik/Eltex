#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include "shared_m.h"


int shm_fd;
Message *shm_ptr; // Указатель на начало разделяемой памяти

sem_t *sem_read_msg;
sem_t *sem_write_msg;

sem_t *count_clients;

void createSharedMemory()
{
    int shm_fd = shm_open(PATHNAME, O_CREAT | O_RDWR, 0666);

    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, BUFSIZE) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shm_ptr = mmap(NULL, BUFSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    memset(shm_ptr, 0, sizeof(Message));
}

void createSemaphores()
{
    sem_read_msg = sem_open(SEMREAD, O_CREAT, 0666, 0);
    if (sem_read_msg == SEM_FAILED)
    {
        perror("sem_open sem_read_msg");
        shm_unlink(PATHNAME);
        exit(EXIT_FAILURE);
    }

    sem_write_msg = sem_open(SEMWRITE, O_CREAT, 0666, 0);
    if (sem_write_msg == SEM_FAILED)
    {
        shm_unlink(PATHNAME);
        sem_close(sem_read_msg);
        perror("sem_open sem_write_msg");
        exit(EXIT_FAILURE);
    }

    count_clients = sem_open(SEMCOUNTCLIENTS, O_CREAT, 0666, 0);
    if (count_clients == SEM_FAILED)
    {
        shm_unlink(PATHNAME);
        sem_close(sem_read_msg);
        sem_close(sem_write_msg);
        perror("sem_open count_clients");
        exit(EXIT_FAILURE);
    }
}

int get_count_clients()
{
    int res;
    sem_getvalue(count_clients, &res);
    return res;
}

int run()
{
    int end_of_shm_ptr = 0;
    sem_post(sem_write_msg);
    
    while (1)
    {
        if (shm_ptr[end_of_shm_ptr].pid_client != 0)
        {
            sem_wait(sem_write_msg);
            
            while (shm_ptr[end_of_shm_ptr].pid_client != 0)
            {
                end_of_shm_ptr++;
            }
            
            sem_post(sem_write_msg);

            printf("Nam message: %d\n", end_of_shm_ptr);
        }
    }
}

void sigint_handler(int signo)
{
    (void)signo;
    printf("\nServer shutting down...\n");

    if (shm_unlink(PATHNAME) == -1)
    {
        perror("shm_unlink");
    }

    if (sem_unlink(SEMWRITE) == -1 || sem_unlink(SEMREAD) == -1 || sem_unlink(SEMCOUNTCLIENTS) == -1)
    {
        perror("sem_unlink");
    }

    exit(EXIT_SUCCESS);
}

int main()
{
    createSharedMemory();
    createSemaphores();
    signal(SIGINT, sigint_handler);

    if (run() == -1)
    {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}