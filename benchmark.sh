#!/bin/bash

check_arguments () {
    if [ "$1" -ne 4 ]; then
        printf "Argument error. You need to pass 4 arguments: \n\t1.: target (zmq/nanomsg) \n\t2.: Protocoll (ipc or tcp) \n\t3.: maximum amount of workers as power (base is 2) \n\t4.: The power of the biggest msg size you want to send (base is 2)\n"
        printf 'A good starting point would me: \n\t: ./benchmark.sh zmq ipc 4 16\n\t-> Results in a max msg size of 64KiB and 128 worker_count\n'
        printf 'Note that this cmq implementation does not work with IPC yet.\n\n'
        kill -s TERM $BENCHMARK_PID
    fi

    if [[ !"$target" == "zmq" && !"$target" == "nanomsg" ]]; then
        echo "Benchmark.sh: Target was set to <$target> which is not valid. Valid options are 'nanomsg' and 'zmq'"
        kill -s TERM $BENCHMARK_PID
    fi

}

trap "exit 1" TERM
BENCHMARK_PID=$$

check_arguments $#
target=$1
protocol=$2
max_worker_power=$3
max_msg_size_power=$4

./compile.sh $target

if [[ $? == 1 ]]; then kill -s TERM $BENCHMARK_PID; fi

# nanomsg?
frontend=tcp://127.0.0.1:5559
backend=tcp://127.0.0.1:5560


if [[ "$protocol" = "ipc" ]]; then
    frontend=ipc://reqrep_fe.ipc
    backend=ipc://reqrep_be.ipc
fi

for (( i=0; i<=$max_worker_power; i++ ))
do
    worker_count=$((2 ** i))
    printf "worker_count: $worker_count\n"
    sleep 0.5
    ./run.sh $frontend $backend $worker_count $max_msg_size_power $target
    # if [[ $? == 1 ]]; then kill -s TERM $BENCHMARK_PID; fi
done