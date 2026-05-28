#include <stdint.h>

struct client
{
    uint32_t ip;
    uint16_t port;
    unsigned int count;
};

struct listClient
{
    struct client data;
    struct listClient *next;
};

// Инициализировать список
struct listClient* createList();
// Добавить клиента в список и вернуть указатель. Если клиент существует, то вернуть указатель на него в списке. 
struct client* addClient(struct listClient *list, struct client client);
// Удалить клиента
int removeClient(struct listClient **list, struct client client);
// Освобождение памяти
void freeListClient(struct listClient *list);
