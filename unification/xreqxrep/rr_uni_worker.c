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
    uint64_t bytes_recieved = 0;
    char delim[] = ";";

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

    printf("\nThroughput in KiBi/second; bytes received; delta_us\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while (1)
    {
        bytes_recieved = 0;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        // #ifdef czmq
        //         char *payload = s_recv(responder);
        // #endif
        //         printf(payload);

        //         char *ptr = strtok(payload, delim);

        //         // we must declare these variables exactly in this sequence
        //         char *tag = strtok(NULL, delim);
        //         printf(tag);
        //         char *client_id = strtok(NULL, delim);
        //         printf(client_id);
        //         int msg_size = atoi(strtok(NULL, delim));
        //         int repetitions = atoi(strtok(NULL, delim));
        //         int client_count = atoi(strtok(NULL, delim));

        int repetitions = 500;
        char *client_id = "123";

        for (int i = 0; i < repetitions; i++)
        {
            //  Wait for next request from client
#ifdef czmq
            char *string = s_recv(responder);
#endif
#ifdef nanomsg
            char *string = ; // TODO
#endif
            // printf(string);

            bytes_recieved = bytes_recieved + strlen(string);

#ifdef czmq
            s_send(responder, client_id);
#endif
            free(string);
        }

        // struct timespec tmp = end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = get_us_past(start, end);
        double kibips = get_kibips(bytes_recieved, delta_us);
        printf("%.6f, %llu, %llu\n", kibips, bytes_recieved, delta_us);
    }
    // }
    //  We never get here, but clean up anyhow

#ifdef czmq
    zmq_close(responder);
    zmq_ctx_destroy(context);
#endif
    return 0;
}
