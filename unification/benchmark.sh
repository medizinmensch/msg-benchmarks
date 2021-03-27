#!/bin/bash


./compile.sh

function check_arguments () {
    if [ "$1" -ne 1 ]; then
        printf "You need to pass 3 arguments: frontend url, backend url, amount of clients. Eg.: \n"
        printf './run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1\n'
        return 0 2>/dev/null || exit "0"
    fi
}

check_arguments $#
max_client_power=$1

for (( i=0; i<=$max_client_power; i++ ))
do
    clients=$((2 ** i))
    printf "Number of clients: $clients\n"
    sleep 5
    ./run.sh "tcp://localhost:5559" "tcp://localhost:5560" $clients
done