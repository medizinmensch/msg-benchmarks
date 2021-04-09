#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

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

char *build_msg(int exp_size, int client_id, int repetitions, int client_count, char *tag)
{
    char header[61]; // len(str(2**64)*3) + 1
    int base = 2;
    int msg_size = pow(base, exp_size);
    char *msg = rand_string_alloc(msg_size);

    snprintf(header, sizeof(header), "%s;%i;%i;%i;%i;", tag, client_id, msg_size, repetitions, client_count); // client_id,msg_size,repetitions
    for (int i = 0; i < strlen(header); i++)
        msg[i] = header[i];

    // printf("Header to send: %s\n", header);

    return msg;
}

// return ns?
uint64_t get_us_past(struct timespec start, struct timespec end)
{
    return ((end.tv_sec - start.tv_sec) * 1000000) + ((end.tv_nsec - start.tv_nsec) / 1000);
}

double get_kibips(int bytes_recieved, uint64_t delta_us)
{
    return ((double)bytes_recieved / 1024) / ((double)delta_us / 1000);
}