#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "driverTask.h"

#define STRSIZE 32

int main(int argc, char *argv[])
{
    char input[STRSIZE], *status;
    char *cmd;
    pid_t pid_create = 0, pid;
    int time_task = 0, res = 0, num_drvs = 0;
    driverInfo driverInfo, *arr_drv = NULL;
    
    while (1)
    {
        printf("> ");
        fgets(input, STRSIZE, stdin);

        input[strcspn(input, "\n")] = '\0';

        cmd = strtok(input, " ");
        if (cmd == NULL)
            continue;

        if (strcmp(cmd, "create_driver") == 0)
        {
            pid_create = create_driver();
            if (pid_create == -1)
            {
                perror("create_driver");
                exit(EXIT_FAILURE);
            }

            printf("> Create new driver with pid: %d\n", pid_create);
        }
        else if (strcmp(cmd, "send_task") == 0)
        {
            cmd = strtok(NULL, " ");
            if (cmd == NULL)
            {
                fprintf(stderr, "> Uncorrected format\n");
                fprintf(stderr, "> Format: send_task [pid] [task_time]\n");
                continue;
            }

            pid = atoi(cmd);
            if (pid == 0)
            {
                fprintf(stderr, "> The pid must be a positive integer\n");
                fprintf(stderr, "> Format: send_task [pid] [task_time]\n");
                continue;
            }

            cmd = strtok(NULL, " ");
            if (cmd == NULL)
            {
                fprintf(stderr, "> Uncorrected format\n");
                fprintf(stderr, "> Format: send_task [pid] [task_time]\n");
                continue;
            }

            time_task = atoi(cmd);
            if (time_task < 0)
            {
                fprintf(stderr, "> The task time must be a non-negative number.\n");
                fprintf(stderr, "> Format: send_task [pid] [task_time]\n");
                continue;
            }

            if (send_task(pid, time_task, &driverInfo) == -1)
            {
                if (driverInfo.status == DRIVER_BUSY)
                {
                    fprintf(stderr, "> The driver with pid %d is busy\n", driverInfo.pid);
                    fprintf(stderr, "> There are %d seconds left to finish\n", driverInfo.task_timer);
                    continue;
                }
                else if (driverInfo.status == DRIVER_UNKNOWN)
                {
                    fprintf(stderr, "> The driver with pid %d was not found\n", pid);
                    continue;
                }
            }

            printf("> Successfully send task to driver with pid %d\n", pid);
        }
        else if (strcmp(cmd, "get_status") == 0)
        {
            cmd = strtok(NULL, " ");
            if (cmd == NULL)
            {
                fprintf(stderr, "> Uncorrected format\n");
                fprintf(stderr, "> Format: get_status [pid]\n");
                continue;
            }

            pid = atoi(cmd);
            if (pid == 0)
            {
                fprintf(stderr, "> The pid must be a positive integer\n");
                fprintf(stderr, "> Format: get_status [pid]\n");
                continue;
            }

            res = get_status(pid, &driverInfo);
            if (driverInfo.status == DRIVER_UNKNOWN)
            {
                fprintf(stderr, "> The driver with pid %d was not found\n", pid);
                continue;
            }
            else if (driverInfo.status == DRIVER_BUSY)
            {
                status = "busy";
                printf("> Driver info:\n> Pid: %d\n> Status: %s\n> Task time: %d", 
                    driverInfo.pid, status, driverInfo.task_timer);
            }
            else if (driverInfo.status == DRIVER_AVAILABLE)
            {
                status = "available";
                printf("> Driver info:\n> Pid: %d\n> Status: %s\n",
                     driverInfo.pid, status);
            }
        }
        else if (strcmp(cmd, "get_drivers") == 0)
        {            
            arr_drv = get_drivers(&num_drvs);
            printf("> Information about drivers:\n");
            for (int i = 0; i < num_drvs; i++)
            {
                if (arr_drv[i].status == DRIVER_BUSY)
                {
                    status = "busy";
                    printf(">\t%d) Pid: %d; Status: %s; Task time: %d\n",
                        i + 1, arr_drv[i].pid, status, arr_drv[i].task_timer);
                }
                else if (arr_drv[i].status == DRIVER_AVAILABLE)
                {
                    status = "available";
                    printf(">\t%d) Pid: %d; Status: %s\n",
                        i + 1, arr_drv[i].pid, status);
                }
            }

            free(arr_drv);
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            exit_drivers();
            break;
        }
        else
        {
            printf("> Unknown command: %s\n", cmd);
            printf("> Available commands:\n");
            printf(">   create_driver\n");
            printf(">   send_task [pid] [task_time]\n");
            printf(">   get_status [pid]\n");
            printf(">   get_drivers\n");
            printf(">   exit\n");
        }
    }

    exit(EXIT_SUCCESS);
}
