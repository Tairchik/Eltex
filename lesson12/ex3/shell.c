#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>
#include "shell.h"

#define TOKEN_DELIM " \n\t\r\a"

int sh_launch(char ***args)
{
    int i = 0, input_fd = 0;
    while (args[i] != NULL)
    {
        pid_t pid;
        int fd[2];
        if (args[i + 1] != NULL)
        {
            if (pipe(fd) == -1)
            {
                perror("pipe");
                return 1;
            }
        }
        pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return 1;       
        }
        else if (pid == 0)
        {
            if (args[i + 1] == NULL) // выполняем команду в конце
            {
                dup2(input_fd, STDIN_FILENO); // читаем из входа предыдущего пайпа или STDIN_FILENO, стандартный поток вывода не трогаем
                close(input_fd);
                if (execvp(args[i][0], args[i]) == -1) 
                {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }    
            }
            else if (i == 0) // выполняем камнду в начале
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO); // чтение не трогаем, а запись меняем на выход текущего пайпа
                close(fd[1]);
                if (execvp(args[i][0], args[i]) == -1) 
                {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            }
            else // выполняем команду в середине
            {
                close(fd[0]);
                dup2(input_fd, STDIN_FILENO); // читаем из входа предыдущего пайпа
                dup2(fd[1], STDOUT_FILENO); // записываем в ввод текущего пайпа
                close(input_fd);
                close(fd[1]);
                if (execvp(args[i][0], args[i]) == -1) 
                {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
            if (i != 0)
            {
                close(input_fd);
            }
            if (args[i + 1] != NULL)
            {
                close(fd[1]);
                input_fd = fd[0]; // запоминаем ввод предыдущего пайпа
            }
        }
        i++;
    }
    for (int j = 0; j < i; j++)
    {
        wait(NULL);
    }
    return 1;
}


int sh_read_command(char **command_out)
{
    int size_str = 32;
    char *str = calloc(sizeof(char), size_str);

    if (!str)
    {
        perror("calloc str");
        return -1;
    }

    int ch;
    int i = 0;
    while (EOF != (ch = fgetc(stdin)) && ch != '\n')
    {
        str[i] = ch;
        if (i + 2 >= size_str)
        {
            size_str += 32;
            char *temp = realloc(str, sizeof(char) * size_str);
            if (!temp)
            {
                perror("realloc temp");
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

char *expand_pipes(const char *input)
{
    int count_pipes = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '|')
            count_pipes++;
    }
    char *output = malloc(strlen(input) + count_pipes * 3 + 1);
    if (!output)
        return NULL;

    int j = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '|')
        {
            output[j++] = ' ';
            output[j++] = '|';
            output[j++] = ' ';
        }
        else
        {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
    return output;
}

// Разбивает строку на токены (лексемы)
char ***sh_parse_command(char **command)
{
    char *prepared_cmd = expand_pipes(*command);
    if (!prepared_cmd)
        return NULL;

    int size_args_array = 16, pos_args_arr = 0;
    char ***array_args = malloc(size_args_array * sizeof(char **));

    if (!array_args)
    {
        perror("malloc");
        free(prepared_cmd);
        return NULL;
    }

    char *token;
    int pos = 0, size_args = 64;
    char **args = malloc(size_args * sizeof(char *));

    if (!args)
    {
        perror("malloc");
        free(prepared_cmd);
        free(array_args);
        return NULL;
    }

    token = strtok(prepared_cmd, TOKEN_DELIM);
    while (token != NULL)
    {
        // Увеличиваем array_args, если не хватает размера
        if (pos_args_arr >= size_args_array - 2) // -2 чтобы не пришлось увеличивать массив после выхода из while
        {
            size_args_array += 16;
            char ***temp = realloc(array_args, size_args_array * sizeof(char **));
            if (!temp)
            {
                perror("realloc");
                while (pos_args_arr >= 0)
                {
                    free(array_args[pos_args_arr]);
                    pos_args_arr--;
                }
                free(array_args);
                free(prepared_cmd);
                return NULL;
            }
            array_args = temp;
        }
        // Новая команда, записываем в array_args и создаем новый args
        if (strcmp(token, "|") == 0)
        {
            args[pos] = NULL;
            array_args[pos_args_arr] = args;
            pos = 0;
            pos_args_arr++;

            size_args = 64;
            args = malloc(size_args * sizeof(char *));
            if (!args)
            {
                perror("malloc");
                while (pos_args_arr > 0)
                {
                    free(array_args[pos_args_arr - 1]);
                    pos_args_arr--;
                }
                free(array_args);
                free(prepared_cmd);
                return NULL;
            }
            token = strtok(NULL, TOKEN_DELIM);
            continue;
        }

        args[pos] = token;
        pos++;
        // Увеличиваем args, если не хватает размера
        if (pos >= size_args - 1)
        {
            size_args += 64;
            char **temp = realloc(args, size_args * sizeof(char *));

            if (!temp)
            {
                perror("realloc");
                while (pos_args_arr > 0)
                {
                    free(array_args[pos_args_arr - 1]);
                    pos_args_arr--;
                }
                free(array_args);
                free(args);
                free(prepared_cmd);
                return NULL;
            }
            args = temp;
        }

        token = strtok(NULL, TOKEN_DELIM);
    }
    args[pos] = NULL;
    array_args[pos_args_arr] = args;
    array_args[pos_args_arr + 1] = NULL;

    free(*command);
    *command = prepared_cmd; 
    return array_args;
}

int sh_cd(char **args)
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
int sh_perfom_cmd(char ***args)
{
    int i = 0;
    while (args[i] != NULL)
    {
        if (args[i][0] == NULL)
        {
            return 1;
        }
        i++;
    }
    if (i == 1)
    {
        if (strcmp("exit", args[0][0]) == 0)
        {
            return 0;
        }
        else if (strcmp("cd", args[0][0]) == 0)
        {
            return sh_cd(args[0]);
        }
    }
   
    return sh_launch(args);
}

int sh_start()
{
    char *cmd;
    int size_command;
    char ***args;
    int res_perform = 1;

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
        args = sh_parse_command(&cmd);
        res_perform = sh_perfom_cmd(args);
        free(cmd);
        int i = 0;
        while (args[i] != NULL)
        {
            free(args[i]);
            i++;
        }
        free(args);
    }
    return EXIT_SUCCESS;
}