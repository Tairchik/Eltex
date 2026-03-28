#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


/*
Реализовать программу, которая создает неименованный канал и
порождает процесс. Родительский процесс закрывает дескриптор для
чтения из канала и записывает в канал строку “Hi!”, затем ожидает
завершения дочернего процесса. Дочерний процесс закрывает
дескриптор для записи в канал, считывает строку из канала и выводит на
экран.
*/

int main()
{
    int fr;
    int fd[2];
    pipe(fd);

    fr = fork();
    switch (fr)
    {
        case -1:
            exit(EXIT_FAILURE);
            break;
        case 0:
            // Ребенок
            close(fd[1]);
            char b;
            printf("Message from parent: ");
            while (read(fd[0], &b, 1) > 0 && b != '\0')
            {
                printf("%c", b);            
            }
            printf("\n");
            close(fd[0]);
            exit(EXIT_SUCCESS);
            break;

        default:
            // Родитель
            close(fd[0]);
            char *message_in = "Hi";
            write(fd[1], message_in, strlen(message_in) + 1);

            wait(NULL);
            close(fd[1]);
            break;
    }

    exit(EXIT_SUCCESS);    
}