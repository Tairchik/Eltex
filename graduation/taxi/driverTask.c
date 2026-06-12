#include <sys/epoll.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "driverTask.h"

// Код для ответной части
#define RESP_SUCCESS   0
#define RESP_BUSY      1
#define RESP_AVAILABLE 2

// Код команды выхода для процесса-водитель
#define CMD_STATUS 98
#define CMD_EXIT 99

// Структура для ответов родителю
typedef struct {
    int code;       // RESP_*
    int seconds;    // RESP_BUSY
} RespMsg;

// Структура для хранения каналов и pid каждого водителя
struct driverData
{
    pid_t pid;
    int cmd_send_task_fd;   // куда родитель задает задачу водителю
    int cmd_get_status_fd;  // куда родитель задает получить статус водителя
    int resp_fd;            // откуда родитель читает ответы
};

static int step = 10;
static int num_drivers = 0;
static int size_arr_drv = 10;
static struct driverData *arr_drv = NULL;   // Для внутреннего взаимодействия

// Функция, выполняющаяся в дочернем процессе (водитель)
static void driver_service(int cmd_task_read_fd, int cmd_status_read_fd, int resp_write_fd)
{
    int epfd, res, ev_res, seconds = 0, read_cmd = 0;
    int busy = 0; // Флаг занятости: 0 - не занят; 1 - занят 
    time_t start, end; // Для замера времени
    struct epoll_event evlist[2];
    struct epoll_event ev_cmd_task;
    struct epoll_event ev_cmd_status;
    RespMsg resp;

    epfd = epoll_create(1);
    if (epfd == -1)
    {
        exit(EXIT_FAILURE);
    }

    // Подписываем дескриптор на чтение команды "задать задачу"
    ev_cmd_task.events = EPOLLIN;
    ev_cmd_task.data.fd = cmd_task_read_fd;
    res = epoll_ctl(epfd, EPOLL_CTL_ADD, cmd_task_read_fd, &ev_cmd_task);
    
    if (res == -1) 
        exit(EXIT_FAILURE);
        
    // Подписываем дескриптор на чтение команды "получить статус"
    ev_cmd_status.events = EPOLLIN;
    ev_cmd_status.data.fd = cmd_status_read_fd;
    res = epoll_ctl(epfd, EPOLL_CTL_ADD, cmd_status_read_fd, &ev_cmd_status);

    if (res == -1) 
        exit(EXIT_FAILURE);

    while (1)
    {
        if (busy == 0)
        {
            // Ждем команды
            ev_res = epoll_wait(epfd, evlist, 1, -1);
            
            // Команда status
            if (evlist[0].data.fd == cmd_status_read_fd)
            {
                // Освобождаем буфер и проверяем, что нет команды exit (завершения)
                read(cmd_status_read_fd, &read_cmd, sizeof(int));
                if (read_cmd == CMD_EXIT)
                {
                    exit(EXIT_SUCCESS);
                }

                resp.code = RESP_AVAILABLE;
                write(resp_write_fd, &resp, sizeof(RespMsg));
            }
            // Команда task
            else
            {
                busy = 1;
                read(cmd_task_read_fd, &seconds, sizeof(int));

                if (seconds <= 0)
                {
                    busy = 0;
                }
                resp.code = RESP_SUCCESS;
                write(resp_write_fd, &resp, sizeof(RespMsg));
            }
        }
        else
        {
            // Выполняем задачу, на seconds секунд
            // Засекаем время, чтобы потом продолжить, если процесс прервали
            start = time(NULL);

            ev_res = epoll_wait(epfd, evlist, 1, seconds * 1000);
            
            end = time(NULL);
            
            // Время выполнение истекло
            if (ev_res == 0)
            {
                busy = 0;
                continue;
            }
            
            // Получили команду
            // Вычисляем оставшееся время
            seconds = seconds - (int)(end - start);
            if (seconds <= 0)
            {
                busy = 0;
                continue;
            }
            // Команда status
            if (evlist[0].data.fd == cmd_status_read_fd)
            {
                // Освобождаем буфер и проверяем, что нет команды exit (завершения)
                read(cmd_status_read_fd, &read_cmd, sizeof(int));
                if (read_cmd == CMD_EXIT)
                {
                    exit(EXIT_SUCCESS);
                }

                resp.code = RESP_BUSY;
                resp.seconds = seconds;
                write(resp_write_fd, &resp, sizeof(RespMsg));
            }
            // Команда task
            else
            {
                int dummy = 0;
                read(cmd_task_read_fd, &dummy, sizeof(int));

                // Отправляем ошибку
                resp.code = RESP_BUSY;
                resp.seconds = seconds;
                write(resp_write_fd, &resp, sizeof(RespMsg));
            }
        }
    }
}

pid_t create_driver()
{
    int fr;
    int fd_cmd_task[2], fd_cmd_status[2], fd_resp[2];

    pipe(fd_cmd_task);
    pipe(fd_cmd_status);
    pipe(fd_resp);

    fr = fork();

    if (fr == 0)
    {
        // Процесс-водитель
        close(fd_cmd_task[1]);
        close(fd_cmd_status[1]);
        close(fd_resp[0]);

        driver_service(fd_cmd_task[0], fd_cmd_status[0], fd_resp[1]);
        exit(EXIT_SUCCESS);
    }
    else if (fr == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Родитель
    close(fd_cmd_task[0]);
    close(fd_cmd_status[0]);
    close(fd_resp[1]);

    // Если это первое создание, инициализируем массивы с данными водителей
    if (num_drivers == 0)
    {
        arr_drv = malloc(sizeof(struct driverData) * size_arr_drv);
        if (!arr_drv)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }
    // Если массивы заполнены
    else if (num_drivers == size_arr_drv)
    {
        size_arr_drv += step;
        arr_drv = realloc(arr_drv, size_arr_drv * sizeof(struct driverData));
        if (!arr_drv)
        {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }

    arr_drv[num_drivers].pid = fr;
    arr_drv[num_drivers].cmd_send_task_fd = fd_cmd_task[1];
    arr_drv[num_drivers].cmd_get_status_fd = fd_cmd_status[1];
    arr_drv[num_drivers].resp_fd = fd_resp[0];

    num_drivers++;

    return fr;
}

static int find_driver(pid_t pid, struct driverData** outData)
{
    for (int i = 0; i < num_drivers; i++)
    {
        if (arr_drv[i].pid == pid)
        {
            *outData = &arr_drv[i];
            return pid;
        }
    }
    return -1;
}

int send_task(pid_t pid, int seconds, driverInfo *drv_info)
{
    if (!drv_info)
    {
        fprintf(stderr, "send_task: drv_info is null");
        exit(EXIT_FAILURE);
    }
    struct driverData *drv_data;
    int pid_fn;
    RespMsg resp;
    
    pid_fn = find_driver(pid, &drv_data);
    if (pid_fn == -1)
    {
        drv_info->pid = 0;
        drv_info->status = DRIVER_UNKNOWN;
        drv_info->task_timer = 0;
        return -1;
    }
    
    write(drv_data->cmd_send_task_fd, &seconds, sizeof(int));
    read(drv_data->resp_fd, &resp, sizeof(RespMsg));
    if (resp.code == RESP_SUCCESS)
    {
        return 1;
    }
    else if (resp.code == RESP_BUSY)
    {
        drv_info->pid = pid_fn;
        drv_info->status = DRIVER_BUSY;
        drv_info->task_timer = resp.seconds;
    }

    return -1;
}

int get_status(pid_t pid, driverInfo *drv_info)
{
    if (!drv_info)
    {
        fprintf(stderr, "send_task: drv_info is null");
        exit(EXIT_FAILURE);
    }

    struct driverData *drv_data;
    int pid_fn, msg_wr;
    RespMsg resp;
    
    pid_fn = find_driver(pid, &drv_data);
    if (pid_fn == -1)
    {
        drv_info->pid = 0;
        drv_info->status = DRIVER_UNKNOWN;
        drv_info->task_timer = 0;
        return -1;
    }
    
    msg_wr = CMD_STATUS;
    write(drv_data->cmd_get_status_fd, &msg_wr, sizeof(int));
    read(drv_data->resp_fd, &resp, sizeof(RespMsg));

    drv_info->pid = pid_fn;
    drv_info->status = resp.code == RESP_BUSY ? DRIVER_BUSY : DRIVER_AVAILABLE;
    drv_info->task_timer = resp.code == RESP_BUSY ? resp.seconds : 0;
    
    return 1;
}

driverInfo *get_drivers(int *num_drvs)
{
    if (num_drivers == 0)
    {
        *num_drvs = 0;
        return NULL;
    }

    driverInfo *result = malloc(sizeof(driverInfo) * num_drivers);
    if (!result)
    {
        *num_drvs = 0;
        return NULL;
    }

    for (int i = 0; i < num_drivers; i++)
        get_status(arr_drv[i].pid, &result[i]);

    *num_drvs = num_drivers;
    return result;
}

void exit_drivers()
{
    int cmd = CMD_EXIT;
    for (int i = 0; i < num_drivers; i++)
    {
        write(arr_drv[i].cmd_get_status_fd, &cmd, sizeof(int));

        // ждём завершения дочернего процесса
        waitpid(arr_drv[i].pid, NULL, 0);
        
        // закрываем все дескрипторы
        close(arr_drv[i].cmd_send_task_fd);
        close(arr_drv[i].cmd_get_status_fd);
        close(arr_drv[i].resp_fd);
    }

    free(arr_drv);
    arr_drv = NULL;
    num_drivers = 0;
    size_arr_drv = 10;
}