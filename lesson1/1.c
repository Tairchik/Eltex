#include <stdio.h>

void printBinary(unsigned int number)
{
    if (number == 0) 
    {
        printf("0\n");
        return;
    }

    unsigned int temp = 1;
    while (temp <= number/2) 
    {
        temp <<= 1; 
    }

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

//Вывести двоичное представление целого положительного числа, используя битовые операции (число вводится с клавиатуры).
int main() 
{
    unsigned int number;
    scanf("%d", &number);
    printBinary(number);

    return 0;
}