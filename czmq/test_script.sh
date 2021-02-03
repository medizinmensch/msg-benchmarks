#!/bin/sh

gcc req.c -lczmq -lm -o ./dist/req
gcc -lczmq rep.c -o ./dist/rep

# ./dist/rep "ipc:///tmp/feeds/0" &
# ./dist/req "ipc:///tmp/feeds/0"

# ./dist/rep "tcp://*:5555" &
./dist/rep "tcp://localhost:5556" &
./dist/req "tcp://localhost:5556" 
