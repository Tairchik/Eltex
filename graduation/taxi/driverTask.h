#include <unistd.h>

#define DRIVER_UNKNOWN 0
#define DRIVER_BUSY 1
#define DRIVER_AVAILABLE 2

typedef struct driverInfo
{
    int status;     // Состояние водителя
    int task_timer; // Оставшиеся время, только если водитель в состоянии DRIVER_BUSY, иначе 0
    pid_t pid;      // pid водителя, при DRIVER_UNKNOWN равен 0
} driverInfo;

// Создать новый процесс-водитель, возвращает pid процесса, в случае ошибки -1
pid_t create_driver();

/*
 Создать задачу водителю. Если водитель в состоянии
 DRIVER_AVAILABLE, возврат 1, иначе -1.
 Информация отрицательного возврата отображается в drv_info
*/
int send_task(pid_t pid, int seconds, driverInfo *drv_info);

// Получить статус водителя
int get_status(pid_t pid, driverInfo *drv_info);

// Получить статусы всех водителей. num_drvs - общее количество водителей
driverInfo *get_drivers(int *num_drvs);

// Завершение
void exit_drivers();