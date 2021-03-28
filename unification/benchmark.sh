#!/bin/bash

function check_arguments () {
    if [ "$1" -ne 2 ]; then
        printf "Argument error. You need to pass 2 arguments: \n\t1.: maximum amount of clients as power (base is 2) \n\t2.: The power of the biggest msg size you want to send (base is 2)\n"
        printf 'A good starting point would me: \n\t: ./benchmark.sh 4 18\n'
        return 0 2>/dev/null || exit "0"
    fi
}


./compile.sh

check_arguments $#
max_client_power=$1
max_msg_size_power=$2
for (( i=0; i<=$max_client_power; i++ ))
do
    clients=$((2 ** i))
    printf "Number of clients: $clients\n"
    sleep 1
    ./run.sh "tcp://localhost:5559" "tcp://localhost:5560" $clients $max_msg_size_power
done