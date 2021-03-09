#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <stdbool.h>
#include <inttypes.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#define NODE0 "node0"
#define NODE1 "node1"
// #define DEBUG

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

void fatal(const char *func)
{
        fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
        exit(1);
}

int signal_close(char *url)
{
        char *msg = "FINALMSG";
        char *buf = NULL;
        int bytes = -1;
        int sock;
        int rv;
#ifdef DEBUG
        printf("Connection closed\n");
#endif
        if ((sock = nn_socket(AF_SP, NN_REQ)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
        {
                fatal("nn_socket");
        }
        if ((rv = nn_connect(sock, url)) < 0)
        {
                fatal("nn_connect");
        }
        if ((bytes = nn_send(sock, msg, strlen(msg) + 1, 0)) < 0)
        {
                fatal("nn_send");
        }
        if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0)
        {
                fatal("nn_recv");
        }
#ifdef DEBUG
        printf("NODE1:\tRECEIVED:\t%s\n", buf);
#endif
        nn_freemsg(buf);
        return (nn_shutdown(sock, 0));
        return 0;
}

int rep(const char *url)
{
        int sock;
        int rv;

        if ((sock = nn_socket(AF_SP, NN_REP)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
        {
                fatal("nn_socket");
        }
        if ((rv = nn_bind(sock, url)) < 0)
        {
                fatal("nn_bind");
        }
        while (true)
        {
                char *buf = NULL;
                int bytes;
                if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0)
                {
                        fatal("nn_recv");
                }
#ifdef DEBUG
                printf("NODE0:\tRECEIVED:\t%s\n", buf);
#endif
                char *msg = buf; //TODO: respond with the same msg
                if (strcmp(buf, "FINALMSG") == 0)
                {
                        printf("BREAKING NOW");
                        break;
                }
#ifdef DEBUG
                printf("NODE0:\tSENDING:\t%s\n", msg);
#endif
                if ((bytes = nn_send(sock, msg, strlen(msg) + 1, 0)) < 0)
                {
                        fatal("nn_send");
                }
                nn_freemsg(buf);
        }
}

int send_msgs(char *msg, int reps, const char *url)
{
        struct timespec start, end;
        char *buf = NULL;
        int bytes = -1;
        int sock;
        int rv;

        if ((sock = nn_socket(AF_SP, NN_REQ)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
        {
                fatal("nn_socket");
        }
        if ((rv = nn_connect(sock, url)) < 0)
        {
                fatal("nn_connect");
        }
#ifdef DEBUG
        printf("NODE1:\tSENDING:\t%s\n", msg);
#endif

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        for (int request_nbr = 0; request_nbr != reps; request_nbr++)
        {
                if ((bytes = nn_send(sock, msg, strlen(msg) + 1, 0)) < 0)
                {
                        fatal("nn_send");
                }
                if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0)
                {
                        fatal("nn_recv");
                }
#ifdef DEBUG
                printf("NODE1:\tRECEIVED:\t%s\n", buf);
#endif
                nn_freemsg(buf);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
        printf("%" PRIu64 "\n", delta_us);

        return (nn_shutdown(sock, 0));
}

int bench_nanomsg(int reps, const char *conn, int exp)
{
        printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");
        char *msg;
        for (int i = 0; i < exp; i++)
        {
                double size = pow(10, i);
                printf("%d,%.0f,%s,", reps, size, conn);
                msg = rand_string_alloc(size + 1);
                send_msgs(msg, reps, conn);
        }
        return (0);
}

int main(const int argc, const char **argv)
{
        int exp = 6;
        int reps = 10000;

#ifdef DEBUG
        exp = 2;
        reps = 2;
#endif

        if ((argc > 1) && (strcmp(NODE0, argv[1]) == 0))
                return (rep(argv[2]));

        if ((argc > 1) && (strcmp(NODE1, argv[1]) == 0))
                return (bench_nanomsg(reps, argv[2], exp));

        fprintf(stderr, "Usage: reqrep %s|%s <URL> ...\n", NODE0, NODE1);
        return (1);
}