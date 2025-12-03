#include <stdio.h> 
#define N 100
#define M 100

/*
Напишите программу, которая ищет введенной строке (с клавиатуры)
введенную подстроку (с клавиатуры) и возвращает указатель на начало
подстроки, если подстрока не найдена в указатель записывается NULL.
В качестве срок использовать статические массивы.
*/

int main()
{
    char start_str[N];
    char find_str[M];
    fgets(start_str, N, stdin);
    fgets(find_str, M, stdin);

    char *ptr_start_str;
    char *result = NULL;
    char *temp;
    int count = 0;
    ptr_start_str = start_str;
    temp = find_str;
    
    while (1)
    {
        if (*ptr_start_str == '\n')
        {
            if (*temp == '\n')
            {
                result = ptr_start_str - count;
                break;
            }
            break;
        }
        if (*ptr_start_str == *temp)
        {
            temp++;
            count++;
        }
        else
        {
            if (*temp == '\n') 
            {
                result = ptr_start_str - count;
                break;
            }
            ptr_start_str -= count;
            temp = find_str;
            count = 0;
        }
        ptr_start_str++;    
    }
    printf("%p\n", result);
    if (result == NULL)
    {
        return 0;
    }
    // Для проверки выведем оставшиеся символы
    while (*result != '\n')
    {
        printf("%c", *result);
        result++;
    }
    printf("\n");
    return 0;
}