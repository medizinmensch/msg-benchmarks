#include "../helpers/zhelpers.h"
#include "../helpers/helpers.h"

#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#define czmq
// #define nanomsg

uint64_t exchange_data(void *responder, int *exit_msgs, int *response_code)
{
    uint64_t bytes_recieved = 0;
    char delimiter[] = ";";
    size_t max_header_size = 500;
    char header[max_header_size];
    int msg_parts = 10; // determines in how many parts we split the incoming msgs; higher means more data points but also more inacurate

#ifdef czmq
    char *msg_received = s_recv(responder);
#endif

    strncpy(header, msg_received, max_header_size);
    // printf("RAW: %s\n", header);

    // we must declare these variables exactly in this sequence
    char *tag = strtok(header, delimiter);
    int client_id = atoi(strtok(NULL, delimiter));
    int msg_size = atoi(strtok(NULL, delimiter));
    int repetitions = atoi(strtok(NULL, delimiter));
    int client_count = atoi(strtok(NULL, delimiter));
    repetitions = repetitions * client_count;
    if (strcmp(tag, "E") == 0)
        *exit_msgs += 1;

    printf("%s;%i;%i;%i;%i;", tag, client_id, msg_size, repetitions, client_count);

#ifdef czmq
    s_send(responder, header);
#endif

    for (int i = 0; i < (repetitions / 10); i++)
    {
        //  Wait for next request from client
#ifdef czmq
        char *string = s_recv(responder);
#endif
#ifdef nanomsg
        char *string = ; // TODO
#endif

        bytes_recieved = bytes_recieved + strlen(string);

#ifdef czmq
        s_send(responder, header);
#endif
        free(string);
    }

    if ((msg_parts - 3) < *exit_msgs)
        *response_code = 1;

    return bytes_recieved;
}

int main(int argc, char *argv[])
{
    char *connection_string = argv[1];
    int clients_count = atoi(argv[2]);
    struct timespec start, end;
    uint64_t bytes_recieved = 0;
    int response_code = 0;
    int exit_msgs = 0;

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

    printf("\nTag;client_id;msg_size;repetitions;client_count;Throughput_in_KiBi/second;bytes_received;delta_us;\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while (1)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        bytes_recieved = exchange_data(responder, &exit_msgs, &response_code);

        // struct timespec tmp = end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = get_us_past(start, end);
        double kibips = get_kibips(bytes_recieved, delta_us);
        printf("%.5f;%llu;%llu\n", kibips, bytes_recieved, delta_us);
        if (response_code == 1)
            break;
    }
    // }
    //  We never get here, but clean up anyhow

#ifdef czmq
    zmq_close(responder);
    zmq_ctx_destroy(context);
#endif
    printf("Done\n");
    exit(0);
    return 0;
}
