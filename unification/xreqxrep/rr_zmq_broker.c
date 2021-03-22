//  Simple request-reply broker

#include "../helpers/zhelpers.h"

int main (int argc, char *argv[]) 
{
    void *context = zmq_ctx_new ();
    char *conn_frontend = argv[1];
    char *conn_backend = argv[2];


    //  Socket facing clients
    void *frontend = zmq_socket (context, ZMQ_ROUTER);
    int rc = zmq_bind (frontend, conn_frontend);
    assert (rc == 0);

    //  Socket facing services
    void *backend = zmq_socket (context, ZMQ_DEALER);
    rc = zmq_bind (backend, conn_backend);
    assert (rc == 0);

    //  Start the proxy
    zmq_proxy (frontend, backend, NULL);

    //  We never get here...
    zmq_close (frontend);
    zmq_close (backend);
    zmq_ctx_destroy (context);
    return 0;
}
