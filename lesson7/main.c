#include <stdio.h>
#include "calc.h"


void inputTwoNums(float* a, float* b)
{
    printf("Input first number: ");
    scanf("%f", a);
    printf("Input second number: ");
    scanf("%f", b);
}


void addMenu()
{
    float a, b;
    inputTwoNums(&a, &b);
    add(&a, b);
    printf("Summation result: %f\n", a);
}


void subMenu()
{
    float a, b;
    inputTwoNums(&a, &b);
    sub(&a, b);
    printf("Difference result: %f\n", a);
}


void divMenu()
{
    float a, b;
    inputTwoNums(&a, &b);
    if (div(&a, b) != -1)
    {
        printf("Division result: %f\n", a);
    }
}
void mulMenu()
{
    float a, b;
    inputTwoNums(&a, &b);
    mul(&a, b);
    printf("Multiplication result: %f\n", a);
}


void printMenu()
{
    printf("1) Сложение\n");
    printf("2) Вычитание\n");
    printf("3) Умножение\n");
    printf("4) Деление\n");
    printf("5) Выход\n");
}


int main()
{
    int choice = 0;
    while (1)
    {
        printMenu();
        printf("Enter a number from 1 to 5 to select a menu item: ");
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                addMenu();
                break;
            case 2:
                subMenu();
                break;
            case 3:
                mulMenu();
                break;
            case 4:
                divMenu();
                break;
            case 5:
                return 0;            
            default:
                printf("Wrong number\n");
                break;
        }
    }
    
    return 0;
}