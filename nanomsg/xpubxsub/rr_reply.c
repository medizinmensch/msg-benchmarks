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

#define DEBUG

void fatal(const char *func)
{
        fprintf(stderr, "%s: %s\n", func, nn_strerror(nn_errno()));
        exit(1);
}

int rep(const char *url)
{
        int fd;
        int rv;

        printf("rr_reply: at nn_socket\n");
        if ((fd = nn_socket(AF_SP, NN_REP)) < 0) //AF_SP=std socket; NN_REQ=Client for req/rep model
        {
                fatal("nn_socket");
        }
        printf("rr_reply: at nn_bind\n");
        if ((rv = nn_connect(fd, url)) < 0)
        {
                fatal("nn_connect");
        }
        while (true)
        {
                char *buf = NULL; //diff: char username[128];
                int bytes;
                if ((bytes = nn_recv(fd, &buf, NN_MSG, 0)) < 0) //diff: rc = nn_recv (fd, username, sizeof (username), 0);
                {
                        fatal("nn_recv");
                }
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
                if ((bytes = nn_send(fd, msg, strlen(msg) + 1, 0)) < 0)
                {
                        fatal("nn_send");
                }
                nn_freemsg(buf);
#ifdef DEBUG
                printf("rr_reply:\tSENDING: <%s>\n", msg);
#endif
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