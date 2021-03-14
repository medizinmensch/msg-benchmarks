#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#define DEBUG

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

int send_msgs(char *msg, int reps, const char *url)
{
        struct timespec start, end;
        char *buf = NULL;
        int bytes = -1;
        int sock;
        int rv;

#ifdef DEBUG
        printf("\nrr_request: at nn_socket\n");
#endif
        if ((sock = nn_socket(AF_SP, NN_REQ)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
        {
                fatal("nn_socket");
        }
#ifdef DEBUG
        printf("rr_request: at nn_connect\n");
#endif
        if ((rv = nn_connect(sock, url)) < 0)
        {
                fatal("nn_connect");
        }
        usleep(1000);
#ifdef DEBUG
        printf("rr_request: Start sending:<%s>\n", msg);
// #ifdef DEBUG
#endif

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        for (int request_nbr = 0; request_nbr != reps; request_nbr++)
        {
#ifdef DEBUG
                printf("rr_request: at nn_send, with request_nbr <%i> of <%i>\n", request_nbr + 1, reps);
#endif
                if ((bytes = nn_send(sock, msg, strlen(msg) + 1, 0)) < 0)
                {
                        fatal("nn_send");
                }
#ifdef DEBUG
                printf("rr_request: at nn_recv");
#endif
                if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0)
                {
                        fatal("nn_recv");
                }
#ifdef DEBUG
                printf("rr_request:RECEIVED: <%s>\n", buf);
#endif
                nn_freemsg(buf);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
#ifdef DEBUG
        printf("took: %" PRIu64 "s\n", delta_us);
#endif

        return (nn_shutdown(sock, 0));
}

int bench_nanomsg(int reps, const char *conn, int exp)
{
        // printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");
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
        const char *connection_string = argv[1];

#ifdef DEBUG
        exp = 3;
        reps = 10;
        printf("rr_request: Connection string: %s\n", connection_string);
#endif

        return (bench_nanomsg(reps, connection_string, exp));

        return (1);
}