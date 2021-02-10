#!/usr/bin/env bash

script_dirb=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

gcc reqrep.c -lnanomsg -lm -o ./dist/reqrep

./dist/reqrep node0 tcp://127.0.0.1:5555 & node0=$! && sleep 1
./dist/reqrep node1 tcp://127.0.0.1:5555
kill $node0

./dist/reqrep node0 ipc:///tmp/reqrep.ipc & node0=$! && sleep 1
./dist/reqrep node1 ipc:///tmp/reqrep.ipc
kill $node0

# ./dist/reqrep node0 inproc://test & node0=$! && sleep 1
# ./dist/reqrep node1 inproc://test
# kill $node0