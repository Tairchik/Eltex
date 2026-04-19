#include <semaphore.h>

#define BUFF_SIZE 64
#define PORTLISTEN 12345

int service_server(int port, sem_t *sem);
int listen_server();
