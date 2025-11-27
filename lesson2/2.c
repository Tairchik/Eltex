#include <stdio.h>

#define N 5


// Задание: вывести заданный массив размером N в обратном порядке.
int main() 
{
    int array[N];
    
    // Ввод массива
    for (int i = 0; i < N; i++)
    {
        scanf("%d", &array[i]);
    }
    
    // Вывод в обратном порядке
    for (int i = N - 1; i >= 0; i--)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
    return 0;
}