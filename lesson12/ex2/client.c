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
    while (1) // Если файла не существует, мы ждем, пока сервер его не создаст
    {
        fd = open(path_named_pipe, O_RDONLY);
        
        if (fd < 0)
        {
            if (errno != ENOENT)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            continue;
        }
        break;
    }
    
    char message[10];

    int b = read(fd, message, sizeof(message));
    message[b] = '\0';
    printf("Message: %s\n", message);
    close(fd);

    remove(path_named_pipe);

    exit(EXIT_SUCCESS);    
}