#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>
#include "shell.h"

#define TOKEN_DELIM " \n\t\r\a" 


int sh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();

    switch (pid)
    {
        case -1:
            perror("sh_launch: fork");
            break;
        case 0:
            if (execvp(args[0], args) == -1) 
            {
                perror("sh_launch: execvp");
            }
            break;
        default:
            // Нужно, если процесс приостановлен - синхронизация ожидания
            do 
            {
                wpid = waitpid(pid, &status, WUNTRACED); 
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            break;
    }

    return 1;
}

int sh_read_command(char** command_out)
{
    int size_str = 32;
    char* str = calloc(sizeof(char), size_str);

    if (!str)
    {
        perror("readCommand: calloc str");
        return -1;
    }

    int ch;
    int i = 0;
    while(EOF != (ch = fgetc(stdin)) && ch != '\n')
    {
        str[i]=ch;
        if(i + 2 >= size_str)
        {
            size_str += 32;
            char* temp = realloc(str, sizeof(char) * size_str);
            if (!temp)
            {
                perror("readCommand: realloc temp");
                free(str);
                return -1;
            }
            str = temp;
        }
        i++;
    }
    
    str[i] = '\0'; 
    *command_out = str;
    return i;
}

// Разбивает строку на токены (лексемы)
char** sh_parse_command(char* command)
{
    char ch;
    int size_args = 64;
    char** args = malloc(size_args * sizeof(char*));
    int pos = 0, start_pos = 0;
    
    if(!args)
    {
        perror("sh_parse_command: args malloc");
        return NULL;
    }

    char* token = strtok(command, TOKEN_DELIM);
    while (token != NULL) 
    {
        args[pos] = token;
        pos++;

        if (pos >= size_args) 
        {
            size_args += 64;
            char** temp = realloc(args, size_args * sizeof(char*));
            if (!temp) 
            {
                perror("sh_parse_command: realloc");
                free(args);
                return NULL;
            }
            args = temp;
        }

        token = strtok(NULL, TOKEN_DELIM);
    }

    args[pos] = NULL; // Как символ конца массива указателей

    return args;
}

int sh_cd(char** args)
{
    if (args[1] == NULL) 
    {
        fprintf(stderr, "sh_cd: waiting argument for \"cd\"\n");
    } 
    else 
    {
        if (chdir(args[1]) != 0) 
        {
            perror("sh_cd: chdir");
        }
    }
    return 1;
}

// Выполнение команд
int sh_perfom_cmd(char** args)
{
    if (args[0] == NULL) 
    {
        return 1;
    }
    else if (strcmp("exit", args[0]) == 0)
    {
        return 0;
    }
    else if (strcmp("cd", args[0]) == 0)
    {
        // Отдельная реализация для смены директории, т.к
        // если делегировать это дочернему процессу, 
        // то изменения остануться только в нем, а значит в 
        // реальности пользователь будет в директории, которая была изначально
        // сохранена в родительском процессе
        return sh_cd(args);
    }
    return sh_launch(args);
}

int sh_start()
{
    char* cmd;
    int size_command;
    char** args;
    int res_perform = 1;
    char* cwd;

    while (res_perform)
    {
        char *cwd = getcwd(NULL, 0);
        if (cwd != NULL) 
        {
            printf("%s>", cwd);
            free(cwd);
        }
        else
        {
            perror("sh_sart: getcwd");
            break;
        }
        fflush(stdin);
        size_command = sh_read_command(&cmd);
        if (size_command == -1)
        {
            perror("sh_start: read command");
            continue;
        }
        args = sh_parse_command(cmd);
        res_perform = sh_perfom_cmd(args);
    }

    int i = 0;
    free(args);
    free(cmd);
    return EXIT_SUCCESS;
}