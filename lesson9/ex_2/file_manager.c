#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "file_manager.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int my_sort(const struct dirent **a, const struct dirent **b) 
{
    if (strcmp((*a)->d_name, "..") == 0) return -1;
    if (strcmp((*b)->d_name, "..") == 0) return 1;
    if ((*a)->d_type == 4 && (*b)->d_type != 4) return -1;
    if ((*a)->d_type != 4 && (*b)->d_type == 4) return 1;
    return strcoll((*a)->d_name, (*b)->d_name);
}

int my_filter(const struct dirent *entry) 
{
    if (strcmp(entry->d_name, ".") == 0) return 0;
    return 1;
}

int my_filter_for_head(const struct dirent *entry) 
{
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) return 0;
    return 1;
}

Files* getFilesInfo(char* path)
{
    struct dirent **ptrDirents;
    char *check_root = realpath(path, NULL);
    if (!check_root) 
    {
        perror("getFilesInfo: check_root");
        return NULL;
    }

    int n = -1;
    if (strcmp(check_root, "/") == 0) 
    {
        n = scandir(path, &ptrDirents, my_filter_for_head, my_sort);
    }
    else 
    {
        n = scandir(path, &ptrDirents, my_filter, my_sort);
    }
    
    free(check_root);

    if (n == -1) 
    {
        perror("getFilesInfo: n");
        return NULL;
    }

    Files* files = malloc(sizeof(Files));
    if (!files) {
        for (int i = 0; i < n; i++) free(ptrDirents[i]);
        free(ptrDirents);
        return NULL;
    }

    files->array = malloc(sizeof(InfoFile) * n);
    files->size_arr = n;
    files->__ptrDir = ptrDirents;
    files->current_path = realpath(path, NULL);

    for (int i = 0; i < n; i++) 
    {
        files->array[i].dirent = ptrDirents[i];
        files->array[i].stat = malloc(sizeof(struct stat));

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", files->current_path, ptrDirents[i]->d_name);

        if (stat(full_path, files->array[i].stat) == -1) {
            memset(&files->array[i].stat, 0, sizeof(struct stat));
        }
    }

    return files;
}

int changeDir(Files** files, char* path)
{
    if (!files || !*files || !path) 
    {
        perror("changeDir: invalid arguments");
        return -1;
    }

    char *full_path;
    if (path[0] == '/') 
    {
        full_path = malloc(strlen(path) + 1);
        if (!full_path) {
            free(full_path);
            perror("changeDir: malloc full_path");
            return -1;
        }
        strcpy(full_path, path);
    } 
    else 
    {
        int needed = snprintf(NULL, 0, "%s/%s", (*files)->current_path, path) + 1;
        full_path = malloc(needed);
        if (!full_path) 
        {
            free(full_path);
            perror("changeDir: malloc full_path");
            return -1;
        }
        snprintf(full_path, needed, "%s/%s", (*files)->current_path, path);
    }

    char *shortcut = realpath(full_path, NULL);
    free(full_path);

    if (!shortcut) 
    {
        free(shortcut);
        perror("changeDir: shortcut");
        return -1;
    }

    Files* newData = getFilesInfo(shortcut);
    if (!newData) 
    {
        free(shortcut);
        perror("changeDir: newData");
        return -1;
    } 
    freeFiles(*files);
    free(shortcut);
    *files = newData;
    return 0;
}


void freeFiles(Files* file)
{
    if (!file) return;
    for (int i = 0; i < file->size_arr; i++) 
    {
        free(file->array[i].dirent);
        free(file->array[i].stat);
    }
    free(file->__ptrDir);
    free(file->current_path);
    free(file->array);
    free(file);
}