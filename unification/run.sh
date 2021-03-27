#!/bin/bash

function compile {
    echo "Compiling..."
    gcc ./xreqxrep/rr_uni_client.c -o ./dist/rr_uni_client -lzmq -lm -Dczmq
    gcc ./xreqxrep/rr_uni_worker.c -o ./dist/rr_uni_worker -lzmq -lm -Dczmq
    gcc ./xreqxrep/rr_zmq_broker.c -o ./dist/rr_zmq_broker -lzmq -lm
}

function check_arguments () {
    if [ "$1" -ne 3 ]; then
        printf "You need to pass 3 arguments: frontend url, backend url, amount of clients. Eg.: \n"
        printf './run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1\n'
        return 0 2>/dev/null || exit "0"
    fi
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
exp_power=22
repetitons=5000

printf "Frontent: $frontend\n"
printf "Backend: $backend\n"
printf "exp_power: $exp_power\n"
printf "repetitons $repetitons\n"
printf "Total Clients: $clients\n"
printf "Starting clients: "


## Start clients
for (( i=1; i<=$clients; i++ ))
do
    printf "$i "
    ./dist/rr_uni_client $frontend $i $exp_power $clients &
done
printf "\n"


## Start worker and proxy
./dist/rr_uni_worker $backend $clients &
./dist/rr_zmq_broker "tcp://*:5559" "tcp://*:5560"

