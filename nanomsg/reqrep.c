#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#include <sys/resource.h>
#include <inttypes.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#define NODE0 "node0"
#define NODE1 "node1"

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

int node0(const char *url)
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
        for (;;)
        {
                char *buf = NULL;
                int bytes;
                if ((bytes = nn_recv(sock, &buf, NN_MSG, 0)) < 0)
                {
                        fatal("nn_recv");
                }
                printf("NODE0: RECEIVED REQUEST\n");
                char *msg = "my Response"; //TODO: respond with the same msg
                printf("NODE0: SENDING: %s\n", msg);
                if ((bytes = nn_send(sock, msg, strlen(msg) + 1, 0)) < 0)
                {
                        fatal("nn_send");
                }
                nn_freemsg(buf);
        }
}

int node1(const char *url)
{
        char *msg = "My string";
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
        printf("NODE1: SENDING REQUEST: %s\n", msg);

        int reps = 10;
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
                printf("NODE1: RECEIVED: %s\n", buf);
                nn_freemsg(buf);
        }
        return (nn_shutdown(sock, 0));
}

void bench_nanomsg(int reps, char *conn, bool debug)
{
        printf("Repetitions, Message Size in characters, protocoll used, Elapsed time in us\n");
        char *str;
        for (int i = 0; i < 6; i++)
        {
                double size = pow(10, i);
                printf("%d,%.0f,%s,", reps, size, conn);
                str = rand_string_alloc(size + 1);
                // send_msgs(str, reps, conn, debug);
        }
}

int main(const int argc, const char **argv)
{
        if ((argc > 1) && (strcmp(NODE0, argv[1]) == 0))
                return (node0(argv[2]));

        if ((argc > 1) && (strcmp(NODE1, argv[1]) == 0))
                return (node1(argv[2]));

        fprintf(stderr, "Usage: reqrep %s|%s <URL> ...\n", NODE0, NODE1);
        return (1);
}