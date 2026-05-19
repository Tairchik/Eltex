#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "listClient.h"


struct listClient* createList()
{
    struct listClient* list = malloc(sizeof(struct listClient)); 

    if (!list)
    {
        perror("malloc");
        return NULL;
    }

    list->data.count = 1;
    list->data.ip = 0;
    list->data.port = 0;

    list->next = NULL;

    return list;
}

struct client* addClient(struct listClient *list, struct client client)
{
    if (list->next == NULL)
    {

        if (list->data.ip == client.ip && list->data.port == client.port)
        {
            return &list->data;
        }

        struct listClient* newElement = createList();
        if (!newElement)
        {
            return NULL;
        }

        list->next = newElement;
        newElement->data.count = client.count;
        newElement->data.port = client.port;
        newElement->data.ip = client.ip;

        return &newElement->data;
    }

    // Проверяем есть ли такой элемент в списке
    while (list->next != NULL)
    {
        if (list->data.ip == client.ip && list->data.port == client.port) 
        {
            return &list->data;
        }

        list = list->next;
    }

    if (list->data.ip == client.ip && list->data.port == client.port) 
    {
        return &list->data;
    }

    // Добавляем в конец
    struct listClient* newElement = createList();
    if (!newElement)
    {
        return NULL;
    }
    
    list->next = newElement;
    newElement->data.count = client.count;
    newElement->data.port = client.port;
    newElement->data.ip = client.ip;

    return &newElement->data;
}

void freeListClient(struct listClient *list)
{
    while (list != NULL)
    {        
        struct listClient* tmp = list;
        list = list->next;

        free(tmp);
    }
}
