#include <stdio.h>
#include <malloc.h>
#include "subscriberList.h"
#include <string.h>


// Создать список
struct list* createList(struct subscriber data) 
{
    struct list* head = malloc(sizeof(struct list));
    if (!head)
    {
        return NULL;
    }

    head->next = NULL;
    head->prev = NULL;

    strcpy(head->data.name, data.name);
    strcpy(head->data.second_name, data.second_name);
    strcpy(head->data.tel, data.tel);
    return head;
}

void add(struct list* head, struct subscriber data); // Добавить в список
void pop(char tel); // Удалить из списка по номеру телефона
void find(char tel); // Поиск элемента по номеру телефона
void freeAll(struct subscriber data); // Освободить всю память