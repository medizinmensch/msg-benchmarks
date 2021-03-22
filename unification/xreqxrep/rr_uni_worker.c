#include "../helpers/zhelpers.h"
#include "../helpers/helpers.h"

#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

// #define czmq
// #define nanomsg

int main(int argc, char *argv[])
{
    char *connection_string = argv[1];
    int max_client_id_digits = 8;
    struct timespec start, end;
    int reps = 5000;

#ifndef czmq
#ifndef nanomsg
    printf("No target was specified during compilation. Define either <czmq> or <nanomsg>.");
    return 0;
#endif
#endif

#ifdef czmq
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_connect(responder, connection_string);
#endif

    printf("\nThroughput in KiBi/second\n");
    while (1)
    {
        int bytes_recieved = 0;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        for (int i = 0; i < reps; i++)
        {
            //  Wait for next request from client
#ifdef czmq
            char *string = s_recv(responder);
#endif
            char client[max_client_id_digits + 1];

            for (int c = 0; c < max_client_id_digits; c++)
            {
                client[c] = string[c];
            }
            client[max_client_id_digits] = '\0';

            bytes_recieved = bytes_recieved + strlen(string);

#ifdef czmq
            s_send(responder, client);
#endif
            free(string);
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = get_time_past(start, end);
        // printf("%" PRIu64, delta_us); //724066
        double kibips = get_kibips(bytes_recieved, delta_us) ;

        printf("%.2f\n", kibips);
        bytes_recieved = 0;
    }
    //  We never get here, but clean up anyhow
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}