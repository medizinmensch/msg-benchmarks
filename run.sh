#!/bin/bash


check_arguments () {
    if [ "$1" -ne 5 ]; then
        printf "You need to pass 5 arguments: frontend url, backend url, amount of workers and biggest exp increase of msg size (base is 2) and target (czmq or nanomsg) Eg.: \n"
        printf './run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1 20 czmq\n'
        kill -s TERM $RUN_PID
    fi
}

start_workers () {
    printf "Starting workers: "
    for (( i=1; i<=$worker_count; i++ ))
    do
        printf "$i "
        ./dist/rr_uni_worker $frontend $i $max_exp $worker_count &
    done
    printf "\n"
}

cleanup() {
  pkill -f "rr_.*"
  rm -f ./reqrep_fe.ipc
  rm -f ./reqrep_be.ipc
  printf "\nCleanup OK\n"
  exit 1
}
trap cleanup INT
trap "exit 1" TERM
RUN_PID=$$


check_arguments $#
frontend=$1
backend=$2
worker_count=$3
max_exp=$4
target=$5

printf "Frontent: $frontend\n"
printf "Backend: $backend\n"
printf "max_exp: $max_exp\n"
printf "worker_count: $worker_count\n"


## Start worker_count
# Order: 
#   zmq: 
#       1. workers
#       2. broker
#       3. client
#   nanomsg: 
#       1. broker
#       2. workers (n)
#       3. client (1)


if [[ "$target" = "czmq" ]]; then
    start_workers
    ./dist/rr_zmq_broker "tcp://*:5559" "tcp://*:5560" &
    ./dist/rr_uni_client $backend $worker_count
elif [[ "$target" = "nanomsg" ]]; then
    ./dist/rr_nanomsg_broker $backend $frontend &
    start_workers
    ./dist/rr_uni_client $backend $worker_count
else
    echo "Compile.sh: Target was set to <$target> which is not valid. Valid options are 'nanomsg' and 'czmq'"
fi


cleanup
