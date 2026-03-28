#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define path_named_pipe "./named_pipe"


int main()
{
    int fd;
    if (mkfifo(path_named_pipe, 0777))
    {
        if (errno != EEXIST) { // Если ошибка НЕ "файл уже есть"
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    } 
    fd = open(path_named_pipe, O_WRONLY);
    if (fd <= 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    char *message = "Hi";

    write(fd, message, strlen(message) + 1);
    close(fd);
    exit(EXIT_SUCCESS);    
}