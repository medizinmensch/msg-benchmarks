#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>

#include "zhelpers.h"
#include "helpers.h"

#define czmq
// #define nanomsg

int send_msgs_czmq(char *msg, int reps, char *conn, int client)
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
        // printf("Received reply %d [%s]\n", request_nbr, msg_recvd);
        free(msg_recvd);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint64_t delta_us = get_time_past(start, end);

    // uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    // printf("%" PRIu64 "\n", delta_us);

    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
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
#ifdef czmq
        send_msgs_czmq(msg, repetitions, connection, client_id);
#endif
#ifdef nanomsg
        send_msgs_nanomsg(msg, repetitions, connection, client_id);
#endif

        // i--; // infinity loop
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

    printf(build_msg(500, 44567));
    return 0;
}
