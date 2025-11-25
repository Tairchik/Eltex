#include <stdio.h>

void printBinary(int number)
{
    if (number == 0) 
    {
        printf("0\n");
        return;
    }

    unsigned int temp = 0x80000000;
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

//Вывести двоичное представление целого отрицательного числа, используя битовые операции (число вводится с клавиатуры).
int main() 
{
    int number;
    scanf("%d", &number);
    printBinary(number);

    return 0;
}