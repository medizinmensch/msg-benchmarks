#!/bin/bash


# Compilation
echo "Compiling..."
gcc rrclient.c -o ./dist/rrclient -lzmq -lm
gcc rrworker.c -o ./dist/rrworker -lzmq -lm
gcc rrbroker.c -o ./dist/rrbroker -lzmq -lm


## Execution
# Config
frontend="tcp://localhost:5559"
backend="tcp://localhost:5560"
clients=10

printf "Frontent: $frontend\nBackend: $backend\nStarting $clients clients: "

for (( i=1; i<=$clients; i++ ))
do
   printf "$i "
    ./dist/rrclient $frontend $i &
done

printf "\nmsg_size"

./dist/rrworker $backend &
./dist/rrbroker "tcp://*:5559" "tcp://*:5560"
