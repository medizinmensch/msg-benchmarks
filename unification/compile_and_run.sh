#!/bin/bash

function compile {
    echo "Compiling..."
    gcc ./xreqxrep/rr_uni_client.c -o ./dist/rr_uni_client -lzmq -lm -Dczmq
    gcc ./xreqxrep/rr_uni_worker.c -o ./dist/rr_uni_worker -lzmq -lm -Dczmq
    gcc ./xreqxrep/rr_zmq_broker.c -o ./dist/rr_zmq_broker -lzmq -lm
}

function check_arguments () {
    if [ "$#" -ne 3 ]; then
        printf "Compilation stage is done, but you need to pass 3 arguments for execution (frontend connection, backend connection, amount of clients) like this:\n"
        printf './compile_and_run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1\n'
        return 0 2>/dev/null || exit "0"
    fi
}

# trap cleanup EXIT
function cleanup {
  pkill -f "rr_.*"
  printf "\nCleanup OK\n"
}
trap cleanup INT


# MAIN
compile

## Config
check_arguments $1 $2 $3
frontend=$1
backend=$2
clients=$3
exp_power=12
repetitons=5000
printf "Frontent: $frontend\nBackend: $backend\nStarting $clients client(s): "


## Start clients
for (( i=1; i<=$clients; i++ ))
do
    printf "$i "
    ./dist/rr_uni_client $frontend $i $exp_power $clients &
done
printf "\n"


## Start worker and proxy
./dist/rr_uni_worker $backend &
./dist/rr_zmq_broker "tcp://*:5559" "tcp://*:5560"

