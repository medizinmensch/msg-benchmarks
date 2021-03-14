#include <stdio.h>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>


int main(const int argc, const char **argv)
{

    const char* frontend = argv[1];
    const char* backend = argv[2];

    int s1 = nn_socket(AF_SP_RAW, NN_REQ);
    printf("rr_proxy: at nn_bind(s1, %s)\n", frontend);
    nn_bind(s1, frontend);
    int s2 = nn_socket(AF_SP_RAW, NN_REP);
    printf("rr_proxy: at nn_bind(s2, %s)\n", backend);
    nn_bind(s2, backend);
    printf("rr_proxy: at nn_device(s1, s2)\n");
    nn_device(s1, s2);

    printf("rr_proxy: at nn_device(int s1, int s2)\n");
    int nn_device(int s1, int s2);

    return (1);
}
