#include <stdio.h>
#define N 5

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

// Заполнить матрицу числами от 1 до N^2 улиткой
int main() 
{
    int array[N][N];
    
    int minRow = 0, maxRow = N - 1;
    int minCol = 0, maxCol = N - 1;
    int value = 1;
    while (value <= N * N)
    {
        // Заполняем верхнюю строку слева направо
        for (int i = minCol; i <= maxCol; i++)
        {
            array[minRow][i] = value++;
        }
        minRow++;

        // Заполняем правый столбец сверху вниз
        for (int i = minRow; i <= maxRow; i++)
        {
            array[i][maxCol] = value++;
        }
        maxCol--;

        // Заполняем нижнюю строку справа налево
        for (int i = maxCol; i >= minCol; i--)
        {
            array[maxRow][i] = value++;
        }
        maxRow--;

        // Заполняем левый столбец снизу вверх
        for (int i = maxRow; i >= minRow; i--)
        {
            array[i][minCol] = value++;
        }
        minCol++;
    }

    // Ввод массива
    printArray(array);
}