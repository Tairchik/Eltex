#include <stdio.h>
#include <malloc.h>
#include "subscriberList.h"
#include <string.h>


// Инициализировать список
void createList(struct list* collection) 
{
    collection->head = NULL;
    collection->tail = NULL;
    collection->size = 0;
}


struct elementList* createElemnet(struct subscriber data)
{
    struct elementList* element = malloc(sizeof(struct elementList));

    if (!element) return NULL;

    strcpy(element->data.name, data.name);
    strcpy(element->data.second_name, data.second_name);
    strcpy(element->data.tel, data.tel);
    
    element->next = NULL;
    element->prev = NULL;

    return element;
}

// Добавить в список. 
void add(struct list* collection, struct subscriber data)
{
    struct elementList* newElement = createElemnet(data);
    if (newElement == NULL) return;

    // Если список пуст
    if (collection->size == 0) 
    {
        collection->head = newElement;
        collection->tail = newElement;
        collection->size++;
        return;
    }
    
    // Добавляем новый элемент в конец списка и изменим указатель tail в collection на конец списка
    newElement->prev = collection->tail;
    collection->tail->next = newElement;
    collection->tail = newElement;
    collection->size++;
}

// Удалить из списка по номеру телефона
int pop(struct list* collection, char tel[])
{
    struct elementList* temp = collection->head;

    while (temp != NULL)
    {
        if (strcmp(temp->data.tel, tel) == 0)
        {
            if (temp->prev == NULL) // Удалить первый элемент
            {
                collection->head = collection->head->next;
                if (collection->head != NULL)
                    collection->head->prev = NULL;
            }
            else if (temp->next == NULL) // Удалить последний элемент
            {
                collection->tail = collection->tail->prev;
                if (collection->tail != NULL)
                    collection->tail->next = NULL;
            }
            else // Удалить из середины
            {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            // Освобождаем память
            free(temp);
            collection->size--;
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// Поиск элемента по номеру телефона
struct subscriber* find(struct list* collection, char tel[])
{
    struct elementList* temp = collection->head;
    while (temp != NULL)
    {
        if(strcmp(temp->data.tel, tel) == 0) 
        {
           return &(temp->data);
        }
        temp = temp->next;
    }
    printf("The subscriber was not found.\n", tel);
    return NULL;
}

void freeAll(struct list* collection)
{
    if (collection->head == NULL)
    {
        return;
    }
    if (collection->head->next == NULL)
    {
        free(collection->head);
        return;
    } 

    struct elementList* head = collection->head;
    struct elementList* temp = head;
    
    while (temp != NULL)
    {
        temp = temp->next;
        free(head);
        head = temp;
    }
}