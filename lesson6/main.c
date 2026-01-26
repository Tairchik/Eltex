#include <stdio.h>
#include <malloc.h>
#include "subscriberList.h"
#include <string.h>


int main()
{
    struct list *head;
    struct subscriber data;

    strcpy(data.name, "Tair");
    strcpy(data.tel, "+233123");
    strcpy(data.second_name, "Bik");
    
    head = createList(data);
    printf("%s %s %s", head->data.name, head->data.second_name, head->data.tel);

    strcpy(data.name, "NewName");

    printf("%s %s %s", head->data.name, head->data.second_name, head->data.tel);
    free(head);
    return 0; 
}