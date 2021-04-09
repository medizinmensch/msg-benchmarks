#!/bin/bash

function check_arguments () {
    if [ "$1" -ne 3 ]; then
        printf "Argument error. You need to pass 3 arguments: \n\t1.: target (czmq/nanomsg) \n\t2.: maximum amount of clients as power (base is 2) \n\t3.: The power of the biggest msg size you want to send (base is 2)\n"
        printf 'A good starting point would me: \n\t: ./benchmark.sh czmq 4 16\n\t-> Results in a max msg size of 64KiB and 128 clients'
        return 0 2>/dev/null || exit "0"
    fi
}


check_arguments $#
target=$1
max_client_power=$2
max_msg_size_power=$3

./compile.sh $target

# nanomsg?
frontend=tcp://127.0.0.1:5559
backend=tcp://127.0.0.1:5560

protocol=ipc #tcp/ipc

if [[ "$protocol" = "ipc" ]]; then
    frontend=ipc://reqrep_fe.ipc
    backend=ipc://reqrep_be.ipc
fi

for (( i=0; i<=$max_client_power; i++ ))
do
    clients=$((2 ** i))
    printf "Number of clients: $clients\n"
    sleep 0.5
    ./run.sh $frontend $backend $clients $max_msg_size_power $target
done