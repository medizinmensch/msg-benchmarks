#!/bin/bash

function compile {
    echo "Compiling..."
    gcc ./xreqxrep/rr_uni_client.c -o ./dist/rr_uni_client -lzmq -lm -Dczmq
    gcc ./xreqxrep/rr_uni_worker.c -o ./dist/rr_uni_worker -lzmq -lm -Dczmq
    gcc ./xreqxrep/rr_zmq_broker.c -o ./dist/rr_zmq_broker -lzmq -lm
}

function check_arguments () {
    if [ "$1" -ne 5 ]; then
        printf "You need to pass 5 arguments: frontend url, backend url, amount of clients and biggest exp increase of msg size (base is 2) and target (czmq or nanomsg) Eg.: \n"
        printf './run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1 20 czmq\n'
        return 0 2>/dev/null || exit "0"
    fi
}

function start_clients () {
    for (( i=1; i<=$clients; i++ ))
    do
        printf "$i "
        ./dist/rr_uni_client $1 $i $2 $3 &
    done
    printf "\n"
}

function cleanup {
  pkill -f "rr_.*"
  printf "\nCleanup OK\n"
}
trap cleanup INT


check_arguments $#
frontend=$1
backend=$2
clients=$3
max_exp=$4
target=$5
repetitons=5000

printf "Frontent: $frontend\n"
printf "Backend: $backend\n"
printf "max_exp: $max_exp\n"
printf "repetitons $repetitons\n"
printf "Total Clients: $clients\n"
printf "Starting clients: "



## Start clients
# Order: 
#   zmq: 
#       1. clients
#       2. broker
#       3. worker
#   nanomsg: 
#       1. broker
#       2. worker
#       3. clients


if [[ "$target" = "czmq" ]]; then
    start_clients $frontend $max_exp $clients
    ./dist/rr_zmq_broker "tcp://*:5559" "tcp://*:5560" &
    ./dist/rr_uni_worker $backend $clients
elif [[ "$target" = "nanomsg" ]]; then
    ./dist/rr_nanomsg_broker $backend $frontend &
    start_clients $frontend $max_exp $clients
    ./dist/rr_uni_worker $backend $clients
else
    echo "Compile.sh: Target was set to <$target> which is not valid. Valid options are 'nanomsg' and 'czmq'"
fi


cleanup
