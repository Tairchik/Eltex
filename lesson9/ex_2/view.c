#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include "file_manager.h"


int main() 
{
    Files* data = getFilesInfo("/");

    for (int i = 0; i < data->size_arr; i++)
    {
        printf("%s %u %ld\n", data->array[i].dirent->d_name, data->array[i].dirent->d_type, data->array[i].stat->st_atime); 
    }
    puts("---------------");

    changeDir(&data, "home");
    for (int i = 0; i < data->size_arr; i++)
    {
        printf("%s %u %ld\n", data->array[i].dirent->d_name, data->array[i].dirent->d_type, data->array[i].stat->st_atime); 

    }

    freeFiles(data);
    return 0;
}