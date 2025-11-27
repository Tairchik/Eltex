#include <stdio.h>
#define N 3

// Вычисляет количество цифр в числе
int countDigits(int num) 
{
    if (num == 0) return 1;
    
    int count = 0;
    int n = num;
    while (n > 0) 
    {
        count++;
        n /= 10;
    }
    
    return count;
}

// Выводит квадратную матрицу с выравниванием по максимальному элементу массива
void printArray(int arr[][N])
{
    int maxLength = countDigits(N*N);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%*d ", maxLength, arr[i][j]);
        }
        printf("\n");
    }
}

// Задание: вывести квадратную матрицу по заданному N

int main() 
{
    int array[N][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            array[i][j] = j + N * i + 1;
        }
    }
    printArray(array);
    return 0;
}