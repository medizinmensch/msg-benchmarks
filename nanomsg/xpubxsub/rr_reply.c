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

// #define DEBUG

void fatal(const char *func)
{
        fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
        exit(1);
}

int rep(const char *url)
{
        struct timespec start, end;
        int reps = 500;

        int file_descr = -1;
        int endpoint_id;

        printf("rr_reply: at nn_socket\n");
        if ((file_descr = nn_socket(AF_SP, NN_REP)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
                fatal("nn_socket");
        printf("rr_reply: at nn_bind\n");
        if ((endpoint_id = nn_connect(file_descr, url)) < 0)
                fatal("nn_connect");
        while (true)
        {
                int bytes_recieved = 0;
                clock_gettime(CLOCK_MONOTONIC_RAW, &start);

                for (int i = 0; i < reps; i++)
                {

                        char *buf = NULL; //diff: char username[128];
                        int bytes;
                        if ((bytes = nn_recv(file_descr, &buf, NN_MSG, 0)) < 0) //diff: rc = nn_recv (fd, username, sizeof (username), 0);
                        {
                                fatal("nn_recv");
                        }
                        bytes_recieved = bytes_recieved + strlen(buf);

#ifdef DEBUG
                        printf("rr_reply:RECEIVED:<%s>\n", buf);
#endif
                        char *msg = buf; //TODO: respond with the same msg
                        if (strcmp(buf, "FINALMSG") == 0)
                        {
                                printf("BREAKING NOW");
                                break;
                        }
#ifdef DEBUG
                        printf("rr_reply:\tSENDING: <%s>\n", msg);
#endif
                        if ((bytes = nn_send(file_descr, msg, strlen(msg) + 1, 0)) < 0)
                        {
                                fatal("nn_send");
                        }
                        nn_freemsg(buf);
#ifdef DEBUG
                        printf("rr_reply:\tSENDING: <%s>\n", msg);
#endif
                }

                clock_gettime(CLOCK_MONOTONIC_RAW, &end);
                uint64_t delta_ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
                printf("%" PRIu64 ":", delta_ns);                                                //724066
                double kibips = ((float)bytes_recieved / 1024) / ((float)delta_ns / 1000000000); //

                printf("%.6f\n", kibips);
                bytes_recieved = 0;
        }
        return 1;
}

int main(const int argc, const char **argv)
{
        const char *connection_string = argv[1];

#ifdef DEBUG
        printf("rr_reply: at Connection string: %s\n", connection_string);
#endif

        return (rep(connection_string));
        return (1);
}