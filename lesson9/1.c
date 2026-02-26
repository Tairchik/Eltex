#include <stdio.h>
#include <string.h>
/*
Написать программу, которая создает файл с именем output.txt,
записывает в него строку “String from file”, затем считывает ее из файла
с конца и выводит на экран
*/


int main()
{
    FILE* fp = fopen("output.txt", "w+");
    if (!fp)
    {
        printf("Error occured while opening file \n");
        return 1;
    }

    char words[] = "String from file";
    size_t len = strlen(words);

    size_t st_w = fwrite(words, sizeof(char), len, fp);
    if (st_w != len)
    {
        printf("Error occured while writing file \n");
        return 1;
    }

    char ch;
    putchar('\n');

    for (int i = 1; i <= len; i++)
    {
        if (fseek(fp, -i, SEEK_END) != 0)
        {
            printf("Error while seeking\n");
            break;
        }
        if (fread(&ch, sizeof(char), 1, fp) != 1)
        {
            printf("Error while reading\n");
            break;
        }
        putchar(ch);
    }
    putchar('\n');

    if (fclose(fp) == EOF)
    {
        printf("Error occured while closing file\n");
        return 1;
    }
    return 0;
}