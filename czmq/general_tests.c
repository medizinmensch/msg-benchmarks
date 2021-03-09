
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (size)
    {
        --size;
        for (size_t n = 0; n < size; n++)
        {
            int key = rand() % (int)(sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

char *rand_string_alloc(size_t size)
{
    char *s = malloc(size + 1);
    if (s)
    {
        rand_string(s, size);
    }
    return s;
}

int main()
{
    char *str;
    for (int i = 0; i < 7; i++)
    {
        str = rand_string_alloc(pow(10, i) + 1);
    }
    strcat(str, "\n\n");
    printf(str);
    return 0;
}