#include <stdio.h>
#include <malloc.h>
#include "subscriberList.h"
#include <string.h>


void printMenu()
{
    printf("-------------------\n");
    puts("1) Add a subscriber");
    puts("2) Delete subscriber");
    puts("3) Search for subscribers by phone number");
    puts("4) Output of all records");
    puts("5) Exit");
    printf("-------------------\n");
}

void addMenu(struct list* list)
{
    struct subscriber data;

    // Очищаем буфер ввода
    while(getchar() != '\n');
    
    printf("Name: ");
    scanf("%9s", data.name); 
    
    printf("Second name: ");
    scanf("%9s", data.second_name);
    
    printf("Telephone: ");
    scanf("%9s", data.tel);

    add(list, data);
}

void delete(struct list* list)
{
    while(getchar() != '\n');

    char phone[10];
    printf("Enter the serial phone number to find: ");
    scanf("%9s", phone); 

    if (pop(list, phone) == 1)
    {
        printf("The subscriber with the phone number %s has been deleted.\n", phone);
    }
    else 
    {
        printf("The subscriber with the telephone number %s was not found\n", phone);
    }
}

void printDirectory(struct list* list)
{
    struct elementList* head = list->head;
    
    for(int i = 0; i < list->size; i++)
    {
        printf("Name: %s\n", head->data.name);
        printf("Second Name: %s\n", head->data.second_name);
        printf("Telephone number: %s\n", head->data.tel);
        head = head->next;
    }
}

void findMenu(struct list* list)
{
    while(getchar() != '\n');

    char phone[10];
    printf("Enter the serial phone number to delete: ");
    scanf("%9s", phone); 

    struct subscriber* data = find(list, phone);
    if (data != NULL)
    {
        printf("Name: %s\n", data->name);
        printf("Second Name: %s\n", data->second_name);
        printf("Telephone number: %s\n", data->tel);   
    }
}

int main()
{
    int choice = 0;
    struct list* list;
    createList(list);
    while (1)
    {
        printMenu();
        printf("Enter a number from 1 to 5 to select a menu item: ");
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                addMenu(list);
                break;
            case 2:
                delete(list);
                break;
            case 3:
                findMenu(list);
                break;
            case 4:
                printDirectory(list);
                break;
            case 5:
                freeAll(list);
                return 0;            
            default:
                printf("Wrong number\n");
                break;
        }
    }
    
    return 0;
    
}