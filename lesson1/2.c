#include <stdio.h>

void printBinary(int number)
{
    unsigned int temp = 1U << 31;
    while (temp > 0) 
    {
        if (number & temp) 
        {
            printf("1");
        } 
        else 
        {
            printf("0");
        }
        temp >>= 1; 
    }

    printf("\n");
}

// Вывести двоичное представление целого отрицательного числа, используя битовые операции (число вводится с клавиатуры).
int main() 
{
    int number = 0;
    scanf("%d", &number);

    if (number > 0) 
    {
        printf("The number must be negative\n");
        return 0;
    }

    printBinary(number);

    return 0;
}