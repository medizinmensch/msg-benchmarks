#!/bin/bash

function cleanup {
  echo "Cleanup"
  pkill -f "rr_.*"
}

trap cleanup INT
# trap cleanup EXIT

# Compilation
echo "Compiling..."
gcc ./xreqxrep/rr_uni_client.c -o ./dist/rr_uni_client -lzmq -lm -Dczmq
gcc ./xreqxrep/rr_uni_worker.c -o ./dist/rr_uni_worker -lzmq -lm -Dczmq
gcc ./xreqxrep/rr_zmq_broker.c -o ./dist/rr_zmq_broker -lzmq -lm


## Execution
frontend=$1
backend=$2
clients=$3

if [ "$#" -ne 3 ]; then
    printf "Compilation stage is done, but you need to pass 3 arguments for execution (frontend connection, backend connection, amount of clients) like this:\n"
    printf './compile_and_run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1\n'
    return 0 2>/dev/null || exit "0"
fi

printf "Frontent: $frontend\nBackend: $backend\nStarting $clients client(s): "

for (( i=1; i<=$clients; i++ ))
do
   printf "$i "
    ./dist/rr_uni_client $frontend $i &
done

printf "\n"

./dist/rr_uni_worker $backend &
./dist/rr_zmq_broker "tcp://*:5559" "tcp://*:5560"

