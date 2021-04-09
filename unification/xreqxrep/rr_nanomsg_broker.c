#include <stdio.h>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>


int main(const int argc, const char **argv)
{
    const char* frontend = argv[1];
    const char* backend = argv[2];

    int s1 = nn_socket(AF_SP_RAW, NN_REQ);
    nn_bind(s1, frontend);
    int s2 = nn_socket(AF_SP_RAW, NN_REP);
    nn_bind(s2, backend);
    nn_device(s1, s2);

    int nn_device(int s1, int s2);

    return (1);
}
