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
char *build_msg(size_t msg_size, int client_id)
{
    char *msg = rand_string_alloc(msg_size);

    int client_str_size = 8;
    // 0000000
    char client_str[client_str_size];

    snprintf(client_str, client_str_size, "%007d", client_id); // save with leading zeros

    for (int i = 0; i < client_str_size - 1; i++)
    {
        msg[i] = client_str[i];
    }

    msg[client_str_size - 1] = ';';

    return msg;
}

// uint64_t get_time_past(start_sec, start_ns, end_sec, end_ns)
uint64_t get_time_past(struct timespec start, struct timespec end)
{
    // return ((end_sec - start_sec) * 1000000 + (end_ns - start_ns) / 1000);
    return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
}


uint64_t get_kibips(int bytes_recieved, uint64_t delta_us )
{
    return ((float)bytes_recieved / 1024) / ((float)delta_us / 1000000000);
}