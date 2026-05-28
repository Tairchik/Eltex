#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "listClient.h"

struct listClient *createList()
{
    struct listClient *list = malloc(sizeof(struct listClient));

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

struct client *addClient(struct listClient *list, struct client client)
{
    if (list->next == NULL)
    {

        if (list->data.ip == client.ip && list->data.port == client.port)
        {
            return &list->data;
        }

        struct listClient *newElement = createList();
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
    struct listClient *newElement = createList();
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

int removeClient(struct listClient **list, struct client client)
{
    if (list == NULL || *list == NULL)
    {
        return 1;
    }

    struct listClient *curr = *list;
    struct listClient *prev = NULL;

    while (curr != NULL)
    {
        if (curr->data.port == client.port && curr->data.ip == client.ip)
        {
            // Удаляем первый элемент
            if (prev == NULL)
            {
                if (curr->next == NULL)
                {
                    curr->data.count = 1;
                    curr->data.ip = 0;
                    curr->data.port = 0;
                    return 0;
                }

                *list = curr->next;
                free(curr);
                return 0;
            }

            // Удаляем из середины или конца списка
            else
            {
                prev->next = curr->next;
                free(curr);
                return 0;
            }
        }

        prev = curr;
        curr = curr->next;
    }

    return 1;
}

void freeListClient(struct listClient *list)
{
    struct listClient *tmp;
    while (list != NULL)
    {
        tmp = list;
        list = list->next;

        free(tmp);
    }
}
