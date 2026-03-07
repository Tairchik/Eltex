#include <sys/stat.h>
#include <dirent.h>

#define MAXPATH 512


typedef struct
{
    struct dirent* dirent; // Название, тип, размер файла
    struct stat* stat;  // Время последнего измения
}InfoFile;

typedef struct
{
    InfoFile* array;
    int size_arr;
    char* current_path;
    struct dirent** __ptrDir; // Для очистки
}Files;


Files* getFilesInfo(char* path);
int changeDir(Files** files, char* path);
void freeFiles(Files* file);
