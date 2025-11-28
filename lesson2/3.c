#include <stdio.h>
#define N 3


// Задание: заполнить верхний треугольник матрицы 1, а нижний 0.
int main() 
{
    int array[N][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (j < N - i - 1)
            {
                array[i][j] = 0; 
            }
            else
            {
                array[i][j] = 1;   
            }
        }
    }

     // Ввод массива
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", array[i][j]);

        }
        printf("\n");
    }
}