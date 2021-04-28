// #define zmq
// #define nanomsg

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>

#include "../helpers/helpers.h"

#ifdef zmq
#include "../helpers/zhelpers.h"
#endif

#ifdef nanomsg
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>
#endif

#ifdef zmq
int send_msgs_czmq(char *msg, int reps, char *conn)
{
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, conn);

    int request_nbr;
    for (request_nbr = 0; request_nbr != reps; request_nbr++)
    {
        // printf("Try sending [%s]\n", msg);
        s_send(requester, msg);
        char *msg_recvd = s_recv(requester);
        // printf("Received reply %d [%s]\n", request_nbr, msg_recvd);
        free(msg_recvd);
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}
#endif

#ifdef nanomsg

void fatal(const char *func)
{
    fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
    exit(1);
}

int send_msgs_nanomsg(char *msg, int reps, char *url)
{
    char *buf = NULL;
    int bytes = -1;
    int sock;
    int rv;

    if ((sock = nn_socket(AF_SP, NN_REQ)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
        fatal("nn_socket");
    if ((rv = nn_connect(sock, url)) < 0)
        fatal("nn_connect");
    usleep(1000);

    for (int request_nbr = 0; request_nbr != reps; request_nbr++)
    {
        // printf("rr_request: at nn_send, with request_nbr <%i> of <%i>\n", request_nbr + 1, reps);
        if ((bytes = nn_send(sock, msg, strlen(msg) + 1, 0)) < 0)
            fatal("nn_send");
        // printf("rr_request: at nn_recv");
        if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0)
            fatal("nn_recv");
        // printf("rr_request:RECEIVED: <%s>\n", buf);
        nn_freemsg(buf);
    }

    return (nn_shutdown(sock, 0));
}
#endif

void benchmark(char *url, int worker_count, int client_id, int max_msg_size_exp, int repetitions)
{

    char *tag = "G";
    for (int i = 2; i < max_msg_size_exp; i++)
    {
        if (i == max_msg_size_exp - 1)
            tag = "E"; // for the last few messages
        char *msg = build_msg(i, client_id, repetitions, worker_count, tag);

#ifdef zmq
        send_msgs_czmq(msg, repetitions, url);
#endif

#ifdef nanomsg
        send_msgs_nanomsg(msg, repetitions, url);
#endif
    }
}

int main(int argc, char *argv[])
{
#ifndef zmq
#ifndef nanomsg
    printf("No target was specified during compilation. Define either <zmq> or <nanomsg>.");
    return 0;
#endif
#endif

    char *url = argv[1];
    int client_id = atoi(argv[2]);
    int max_exp = atoi(argv[3]);
    int worker_count = atoi(argv[4]);
    int repetitions = 8192 / worker_count;

    if (max_exp < 4)
    {
        printf("max_exp must be at least 4");
        exit(1);
    }

    benchmark(url, worker_count, client_id, max_exp, repetitions);

    return 0;
}
