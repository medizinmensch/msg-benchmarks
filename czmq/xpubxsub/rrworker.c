//  Hello World worker

#include "zhelpers.h"

#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *connection_string = argv[1];
    void *context = zmq_ctx_new();

    int max_client_id_digits = 8;

    struct timespec start, end;

    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_connect(responder, connection_string);
    int reps = 5000;

    printf("\nThroughput in KiBi/second\n");
    while (1)
    {
        int bytes_recieved = 0;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        for (int i = 0; i < reps; i++)
        {
            //  Wait for next request from client
            char *string = s_recv(responder);
            char client[max_client_id_digits + 1];

            for (int c = 0; c < max_client_id_digits; c++)
            {
                client[c] = string[c];
            }
            client[max_client_id_digits] = '\0';

            bytes_recieved = bytes_recieved + strlen(string);

            s_send(responder, client);
            free(string);
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
        // printf("%" PRIu64, delta_us); //724066
        double kibips = ((float)bytes_recieved / 1024) / ((float)delta_us / 1000000000);

        printf("%.2f\n", kibips);
        bytes_recieved = 0;
    }
    //  We never get here, but clean up anyhow
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}