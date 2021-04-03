#!/bin/bash

function check_arguments () {
    if [ "$1" -ne 3 ]; then
        printf "Argument error. You need to pass 3 arguments: \n\t1.: target (czmq/nanomsg) \n\t2.: maximum amount of clients as power (base is 2) \n\t3.: The power of the biggest msg size you want to send (base is 2)\n"
        printf 'A good starting point would me: \n\t: ./benchmark.sh czmq 4 18\n'
        return 0 2>/dev/null || exit "0"
    fi
}


check_arguments $#
target=$1
max_client_power=$2
max_msg_size_power=$3

./compile.sh $target


# for (( i=0; i<=$max_client_power; i++ ))
# do
#     clients=$((2 ** i))
#     printf "Number of clients: $clients\n"
#     sleep 1
#     ./run.sh "tcp://127.0.0.1:5555" "tcp://127.0.0.1:5556" $clients $max_msg_size_power $target
# done


# target=czmq
for (( i=0; i<=$max_client_power; i++ ))
do
    clients=$((2 ** i))
    printf "Number of clients: $clients\n"
    sleep 1
    ./run.sh "tcp://localhost:5559" "tcp://localhost:5560" $clients $max_msg_size_power $target
done