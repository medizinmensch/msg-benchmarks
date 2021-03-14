#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include <string.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "zhelpers.h"

int _send_msgs(char *msg, int reps, char *conn, int client)
{
    struct timespec start, end;

    void *context = zmq_ctx_new();
    //  Socket to talk to server
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, conn);

    int request_nbr;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (request_nbr = 0; request_nbr != reps; request_nbr++)
    {
        // printf("Try sending [%s]\n", msg);
        s_send(requester, msg);
        char *msg_recvd = s_recv(requester);
        // printf("Received reply %d [%s]\n", request_nbr, msg_recvd);
        free(msg_recvd);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    // printf("%" PRIu64 "\n", delta_us);

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

char *build_msg(size_t msg_size, int client_id)
{
    char *msg = rand_string_alloc(msg_size);

    int client_str_size = 9;
    char client_str[client_str_size];

    snprintf(client_str, client_str_size, "%007d", client_id); // save with leading zeros

    for (int i = 0; i < client_str_size - 1; i++)
    {
        msg[i] = client_str[i];
    }

    msg[client_str_size] = ';';

    return msg;
}

void bench_zmq(int repetitions, char *connection, int client_id)
{
    // printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");
    char *msg;
    for (int i = 1; i < 8; i++)
    {
        double msg_size = pow(10, i);
        // printf("%d,%.0f,%s,", repetitions, msg_size, connection);

        msg = build_msg(msg_size + 1, client_id);
        _send_msgs(msg, repetitions, connection, client_id);
        
        // i--; // infinity loop
    }
}

int main(int argc, char *argv[])
{
    char *conn = argv[1];
    int client_id = strtol(argv[2], NULL, 10);
    
    int reps = 100000;
    bench_zmq(reps, conn, client_id);

    return 0;
}
