#include <stdio.h>
#define N 100


//Написать программу абонентский справочник
struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
};


void printMenu()
{
    printf("-------------------\n");
    puts("1) Добавить абонента");
    puts("2) Удалить абонента");
    puts("3) Поиск абонентов по имени");
    puts("4) Вывод всех записей");
    puts("5) Выход");
    printf("-------------------\n");
}


int add(struct abonent dir[N], int *len)
{
    if (*len == N)
    {
        printf("Phone directory is full");
        return 0;
    }
    
    // Очищаем буфер ввода
    while(getchar() != '\n');
    
    printf("Name: ");
    scanf("%9s", dir[*len].name); 
    
    printf("Second name: ");
    scanf("%9s", dir[*len].second_name);
    
    printf("Telephone: ");
    scanf("%9s", dir[*len].tel);

    (*len)++;
    return 1;
}


int delete(struct abonent dir[N], int *len, int delete_buf[])
{
    int num = 0;
    printf("Enter the serial number to delete: ");
    scanf("%d", &num);
    
    if (num > *len)
    {
        printf("There is no such abonent\n");
        return 0;
    }
    
    int c = 0;
    while (c < *len)
    {
        if (delete_buf[c] == num) 
        {
            printf("This abonent has already been deleted\n");
            return 0;
        }
        c++;
    }

    for (int i = 0; i < sizeof(dir[num - 1].name) - 1; i++)
    {
        dir[num - 1].name[i] = '0';
    }
    for (int i = 0; i < sizeof(dir[num - 1].second_name) - 1; i++)
    {
        dir[num - 1].second_name[i] = '0';
    }
    for (int i = 0; i < sizeof(dir[num - 1].tel) - 1; i++)
    {
        dir[num - 1].tel[i] = '0';
    }

    dir[num - 1].name[sizeof(dir[num - 1].name) - 1] = '\0';
    dir[num - 1].second_name[sizeof(dir[num - 1].second_name) - 1] = '\0';
    dir[num - 1].tel[sizeof(dir[num - 1].tel) - 1] = '\0';

    // Запоминаем какую запись удалили
    c = 0;
    while (1)
    {
        if (delete_buf[c] == 0) 
        {
            delete_buf[c] = num;
            break;
        }
        c++;
    }
    return 1;
}


void printDirectory(struct abonent dir[N], int len, int delete_buf[])
{
    int t = 0;
    char fl = 0;
    printf("\n");
    for (int i = 0; i < len; i++)
    {
        fl = 0;
        t = 0;
        while (delete_buf[t] != 0)
        {
            if (delete_buf[t] - 1 == i) 
            {
                fl = 1;
                break;
            }
            t++;
        }
        if (fl == 1) continue;
        printf("Name: %s\n", dir[i].name);
        printf("Second Name: %s\n", dir[i].second_name);
        printf("Telephone number: %s\n", dir[i].tel);
        printf("Index to delete: %d\n", i + 1);
        printf("-----------------------\n");
    }
}


void findAbobnent(struct abonent dir[N], int len)
{
    char name[10];
    int fl = 0;

    // Очищаем буфер ввода
    while(getchar() != '\n');
    
    printf("Enter name: ");
    scanf("%9s", name);
    
    for (int i = 0; i < len; i++)
    {
        fl = 0;
        for (int j = 0; j < 10; j++)
        {
            if (name[j] == '\0' && dir[i].name[j] == '\0') break;
            if (name[j] != dir[i].name[j])
            {
                fl = 1;
                break;
            }
        }
        if (fl == 0)
        {
            printf("-----------------------\n");
            printf("Name: %s\n", dir[i].name);
            printf("Second Name: %s\n", dir[i].second_name);
            printf("Telephone number: %s\n", dir[i].tel);
            printf("Index to delete: %d\n", i + 1);
        }
    }
}


int main()
{
    int choice = 0;
    struct abonent directory[N];
    int len_directory = 0;
    int delete_buf[N] = {0};

    while (1)
    {
        printMenu();
        printf("Enter a number from 1 to 5 to select a menu item: ");
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                add(directory, &len_directory);
                break;
            case 2:
                delete(directory, &len_directory, delete_buf);
                break;
            case 3:
                findAbobnent(directory, len_directory);
                break;
            case 4:
                printDirectory(directory, len_directory, delete_buf);
                break;
            case 5:
                return 0;            
            default:
                printf("Wrong number\n");
                break;
        }
    }
    
    return 0;
}
