#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
Реализовать программу, которая порождает процесс1 и процесс2, ждет
завершения дочерних процессов. Процесс1 в свою очередь порождает
процесс3 и процесс4 и ждет их завершения. Процесс2 порождает
процесс5 и ждет его завершения. Все процессы выводят на экран свой
pid, ppid
*/
void process(int numProc)
{
    pid_t fr = fork();
    switch (fr)
    {
        case -1:
            exit(EXIT_FAILURE);
            break;
        case 0:
            printf("Process%d pid: %d ppid: %d\n", numProc, getpid(), getppid());
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}


int main()
{
    pid_t fr = fork();
    switch (fr)
    {
        case -1:
            exit(EXIT_FAILURE);
            break;
        case 0:
            printf("Process1 pid: %d ppid: %d\n", getpid(), getppid());
            process(3);
            process(4);

            wait(NULL);
            wait(NULL);

            exit(EXIT_SUCCESS);
            break;
        default:
            pid_t fr2; 
            switch (fr2 = fork())
            {
                case -1:
                    exit(EXIT_FAILURE);
                    break;
                case 0:
                    printf("Process2 pid: %d ppid: %d\n", getpid(), getppid());
                    process(5);
                    wait(NULL);
                    exit(EXIT_SUCCESS);
                    break;
                default:
                    break;
            }

            wait(NULL);
            wait(NULL);
            
            printf("Parent pid: %d ppid: %d\n", getpid(), getppid());
            break;
    }
    exit(EXIT_SUCCESS);   
}