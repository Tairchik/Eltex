#include <stdio.h>


int CountUnits(unsigned int number)
{
    int count = 0;
    while (number > 0) 
    {
        count += number & 1;
        number >>= 1; 
    }
    return count;
}

// Найти количество единиц в двоичном представлении целого положительного числа (число вводится с клавиатуры).
int main() 
{
    unsigned int number;

    scanf("%u", &number);
    int count = CountUnits(number);

    printf("%d\n", count);
    return 0;
}