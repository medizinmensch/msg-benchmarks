#!/bin/bash

# run eg with:

# Compilation
echo "Compiling..."
gcc ./xreqxrep/uni_client.c -o ./dist/uni_client -lzmq -lm -Dczmq
gcc ./xreqxrep/uni_worker.c -o ./dist/uni_worker -lzmq -lm -Dczmq
gcc ./xreqxrep/zmq_broker.c -o ./dist/zmq_broker -lzmq -lm


## Execution
frontend=$1
backend=$2
clients=$3

if [ "$#" -ne 3 ]; then
    printf "Compilation stage is finished, but you need to pass 3 arguments for execution (frontend connection, backend connection, amount of clients) like this:\n"
    printf './compile_and_run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1\n'
    return 0 2>/dev/null || exit "0"
fi

printf "Frontent: $frontend\nBackend: $backend\nStarting $clients client(s): "

for (( i=1; i<=$clients; i++ ))
do
   printf "$i "
    ./dist/uni_client $frontend $i &
done

printf "\n"

./dist/uni_worker $backend &
./dist/zmq_broker "tcp://*:5559" "tcp://*:5560"

pkill -f "uni_client.*"