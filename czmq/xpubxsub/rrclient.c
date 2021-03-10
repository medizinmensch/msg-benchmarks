#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include <string.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "zhelpers.h"

// new
int _send_msgs(char *send_str, int reps, char *conn)
{
    struct timespec start, end;

    void *context = zmq_ctx_new();
    //  Socket to talk to server
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5559");

    int request_nbr;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (request_nbr = 0; request_nbr != 10; request_nbr++)
    {
        s_send(requester, "Hello");
        char *string = s_recv(requester);
        printf("Received reply %d [%s]\n", request_nbr, string);
        free(string);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("%" PRIu64 "\n", delta_us);

    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}

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

// -----------------------------------------------------------

// int signal_close(char *conn)
// {
//     printf("Connection closed\n");
//     zsock_t *requester = zsock_new(ZMQ_REQ);
//     zsock_connect(requester, conn);
//     zstr_send(requester, "FINALMSG");
//     char *str = zstr_recv(requester);
//     zstr_free(&str);
//     zsock_destroy(&requester);
//     return 0;
// }

void bench_zmq(int reps, char *conn, bool debug)
{
    printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");
    char *str;
    for (int i = 0; i < 8; i++)
    {
        double size = pow(10, i);
        printf("%d,%.0f,%s,", reps, size, conn);
        str = rand_string_alloc(size + 1);
        _send_msgs(str, reps, conn);
    }
}

int main(int argc, char *argv[])
{
    char *conn = argv[1];
    printf("%s\n", conn);

    bench_zmq(10000, conn, false);

    // signal_close(conn);

    return 0;
}

