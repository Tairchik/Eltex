#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
Реализовать программу, которая порождает процесс. Родительский
процесс и дочерний выводят на экран свой pid, ppid. Затем родительский
ждет завершения дочернего и выводит его статус завершения.
*/

int main()
{
    pid_t fr;
    int status;

    fr = fork();
    switch (fr)
    {
    case 0:
        printf("Childe pid: %d ppid: %d\n", getpid(), getppid());
        exit(6);
        break;
    case -1:
        exit(EXIT_FAILURE);
        break;
    default:
        printf("Parent pid: %d ppid: %d\n", getpid(), getppid());
        wait(&status);
        printf("Exit status: %d\n", WEXITSTATUS(status));
        break;
    }
    exit(EXIT_SUCCESS);   
}