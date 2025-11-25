#include <stdio.h>

// Поменять в целом положительном числе (типа int) значение третьего байта на введенное пользователем число (изначальное число также вводится с клавиатуры).
int main() 
{
    int number = 0;
    unsigned char third_byte = 0;
    printf("The original number: ");
    scanf("%d", &number);
    printf("The third byte: ");
    scanf(" %hhu", &third_byte);

    unsigned int temp = third_byte << 2 * 8;

    int res = temp | (number & ~(0xFF << 16));

    printf("The result: %d\n", res);

    return 0;
}