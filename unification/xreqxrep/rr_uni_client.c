#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>

#include "../helpers/helpers.h"
#include "../helpers/zhelpers.h"

// #define czmq
// #define nanomsg

// TODO: implement cleanup in case of early termination

int send_msgs_czmq(char *msg, int reps, char *conn)
{
    struct timespec start, end;
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, conn);

    int request_nbr;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (request_nbr = 0; request_nbr != reps; request_nbr++)
    {
        // printf("Try sending [%s]\n", msg);
        s_send(requester, msg);
        char *msg_recvd = s_recv(requester);
        if (msg_recvd == "STOP_PLS")
            // printf("Received reply %d [%s]\n", request_nbr, msg_recvd);
            free(msg_recvd);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint64_t delta_us = get_us_past(start, end);

    // uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    // printf("%" PRIu64 "\n", delta_us);

    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}

void benchmark(char *url, int client_count, int client_id, int max_exp, int repetitions)
{
    // printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");

    char *tag = "G";
    for (int i = 2; i < max_exp; i++)
    {
        if (i == max_exp - 1)
            tag = "E";
        char *msg = build_msg(i, client_id, repetitions, client_count, tag);

#ifdef czmq
        send_msgs_czmq(msg, repetitions, url); // send header for metadata
#endif

#ifdef nanomsg
        send_msgs_nanomsg(msg, repetitions, url, client_id);
#endif
    }
}

int main(int argc, char *argv[])
{
#ifndef czmq
#ifndef nanomsg
    printf("No target was specified during compilation. Define either <czmq> or <nanomsg>.");
    return 0;
#endif
#endif

    char *url = argv[1];
    int client_id = atoi(argv[2]);
    int max_exp = atoi(argv[3]);
    int client_count = atoi(argv[4]);
    int repetitions = 5000;

    if (max_exp < 4)
    {
        printf("max_exp must be at least 4");
        exit(1);
    }

    benchmark(url, client_count, client_id, max_exp, repetitions);

    return 0;
}
