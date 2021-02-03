#include <czmq.h>

int bind_resp(char *argv)
{
    //  Socket to talk to clients
    zsock_t *responder = zsock_new(ZMQ_REP);
    int rc = zsock_bind(responder, "ipc:///tmp/feeds/0");
    // assert (rc == 0);

    char *str;
    while (true)
    {
        str = zstr_recv(responder);
        // printf("Received msg\n");
        // usleep(500000); //  Do some 'work'
        zstr_send(responder, str);

        if (strcmp(str, "FINALMSG") == 0)
        {
            break;
        }
        zstr_free(&str);
    }
    zstr_free(&str);

    zsock_destroy(&responder);

    return 0;
}

int main(int argc, char *argv[])
{
    char *conn = argv[0];

    bind_resp(conn);
    return 0;
}
