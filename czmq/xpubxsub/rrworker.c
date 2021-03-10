//  Hello World worker
//  Connects REP socket to tcp://localhost:5560
//  Expects "Hello" from client, replies with "World"

#include "zhelpers.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *conn = argv[1];

    void *context = zmq_ctx_new();

    //  Socket to talk to clients
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_connect(responder, conn);

    while (1)
    {
        //  Wait for next request from client
        char *string = s_recv(responder);
        char client[7];
        int c = 0;
        while (c < 6)
        {
            client[c] = string[c];
            c++;
        }
        client[6] = '\0';

        printf("Received request from client: [%s]\n", client);

        //  Do some 'work'
        // sleep (1);

        //  Send reply back to client
        s_send(responder, client);
        free(string);

    }
    //  We never get here, but clean up anyhow
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}