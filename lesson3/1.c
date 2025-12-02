#include <stdio.h> 
/*
Поменять в целом положительном числе (типа int) значение третьего
байта на введенное пользователем число (изначальное число также
вводится с клавиатуры) через указатель (не применяя битовые
операции).
*/
int main()
{
    unsigned int number = 0;
    char byte = 0;
    char* ptr;
    
    printf("Start number: ");
    scanf("%u", &number);
    
    printf("Third byte: ");
    scanf("%hhu", &byte);

    ptr = (char *)&number + 2;
    *ptr = byte;
    
    printf("Changed number: %u\n", number);
    return 0;
}