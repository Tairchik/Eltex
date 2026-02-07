#include "calc.h"
#include <stdio.h>


int div(float* divisible, float divider)
{
    if (divider == 0)
    {
        puts("Division by zero");
        return -1;
    }
    *divisible /= divider;
    return 1;
}