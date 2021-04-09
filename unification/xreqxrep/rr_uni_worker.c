// #define czmq
// #define nanomsg

#include "../helpers/helpers.h"

#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#ifdef czmq
#include "../helpers/zhelpers.h"
#endif

#ifdef nanomsg
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>
#endif

#ifdef nanomsg
void fatal(const char *func)
{
    fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
    exit(1);
}
#endif

char *uni_receive(void *responder, int file_descr)
{
#ifdef czmq
    return s_recv(responder);
#endif

#ifdef nanomsg
    char *msg_received = NULL; //diff: char username[128];
    int bytes;
    if ((bytes = nn_recv(file_descr, &msg_received, NN_MSG, 0)) < 0) //diff: rc = nn_recv (fd, username, sizeof (username), 0);
        fatal("nn_recv");
    return msg_received;
#endif
}

void uni_send(void *responder, int file_descr, char *msg)
{
    // printf("BLA\n");
#ifdef czmq
    s_send(responder, msg);
#endif
#ifdef nanomsg
    int bytes;
    if ((bytes = nn_send(file_descr, msg, strlen(msg) + 1, 0)) < 0)
        fatal("nn_send");
#endif
}

void uni_free(void *something)
{
#ifdef czmq
    free(something);
#endif

#ifdef nanomsg
    nn_freemsg(something);
#endif
}

uint64_t exchange_data(int *exit_msgs, int *response_code, int nanomsg_file_descr, void *zmq_responder)
{
    uint64_t bytes_received = 0;
    char delimiter[] = ";";
    size_t max_header_size = 500;
    char header[max_header_size];
    int msg_parts = 10; // determines in how many parts we split the incoming msgs; higher means more data points but also more inacurate

    char *msg_received = uni_receive(zmq_responder, nanomsg_file_descr);

    strncpy(header, msg_received, max_header_size);
    // printf("RAW: %s\n", header);

    uni_free(msg_received);

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

    uni_send(zmq_responder, nanomsg_file_descr, header);

    for (int i = 0; i < (repetitions / 10); i++)
    {
        char *string = uni_receive(zmq_responder, nanomsg_file_descr);
        
        bytes_received = bytes_received + strlen(string);
        uni_send(zmq_responder, nanomsg_file_descr, header);

        uni_free(string);
    }

    if ((msg_parts - 3) < *exit_msgs)
        *response_code = 1;

    return bytes_received;
}

int main(int argc, char *argv[])
{
    char *url = argv[1];
    int clients_count = atoi(argv[2]);
    struct timespec start, end;
    uint64_t bytes_received = 0;
    int response_code = 0;
    int exit_msgs = 0;

    void *responder = NULL; // czmq
    int file_descr = -1;    // nanomsg

#ifndef czmq
#ifndef nanomsg
    printf("No target was specified during compilation. Define either <czmq> or <nanomsg>.");
    return 0;
#endif
#endif

// Initialize msg-libs
#ifdef czmq
    void *context = zmq_ctx_new();
    responder = zmq_socket(context, ZMQ_REP);
    zmq_connect(responder, url);
#endif

#ifdef nanomsg
    int rv;

    if ((file_descr = nn_socket(AF_SP, NN_REP)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
        fatal("nn_socket");
    if ((rv = nn_connect(file_descr, url)) < 0)
        fatal("nn_connect");
#endif

    printf("\nTag;client_id;msg_size;repetitions;client_count;Throughput_in_KiBi/second;bytes_received;delta_us;\n");
    while (1)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        bytes_received = exchange_data(&exit_msgs, &response_code, file_descr, responder);

        // struct timespec tmp = end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = get_us_past(start, end);
        double kibips = get_kibips(bytes_received, delta_us);
        printf("%.5f;%llu;%llu\n", kibips, bytes_received, delta_us);
        if (response_code == 1)
            break;
    }
    //  We never get here, but clean up anyhow

#ifdef czmq
    zmq_close(responder);
    zmq_ctx_destroy(context);
#endif
    printf("Done\n");
    exit(0);
    return 0;
}
