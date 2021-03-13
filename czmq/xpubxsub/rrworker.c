//  Hello World worker
//  Connects REP socket to tcp://localhost:5560
//  Expects "Hello" from client, replies with "World"

#include "zhelpers.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <inttypes.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *conn = argv[1];

    void *context = zmq_ctx_new();

    struct timespec start, end;

    //  Socket to talk to clients
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_connect(responder, conn);

    while (1)
    {
        int bytes_recieved = 0;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        for (int i = 0; i < 5000; i++)
        {
            //  Wait for next request from client
            char *string = s_recv(responder);
            char client[7];

            for (int c = 0; c < 6; c++)
            {
                client[c] = string[c];
            }
            client[6] = '\0';

            bytes_recieved = bytes_recieved + strlen(string);

            //  printf("Received request from client: [%s] of size [%zu]\n", client, strlen(string));

            //  Do some 'work'

            //  Send reply back to client
            s_send(responder, client);
            free(string);

            // printf(".");
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
        printf("%" PRIu64, delta_us);
        float kibips = ((float)bytes_recieved * 976.5625) / (float)delta_us;

        // printf("Received [%zu] bytes in \r %" PRIu64 "\r", bytes_recieved, delta_ns);
        bytes_recieved = 0;
        printf("Current throughput: %.4f KiB/s \n", kibips);
        // printf("%" PRIu64, delta_ns);
        // printf("ns\r");
    }
    //  We never get here, but clean up anyhow
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}