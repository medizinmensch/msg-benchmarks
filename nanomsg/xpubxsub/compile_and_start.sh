#!/usr/bin/env bash

script_dirb=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

gcc rr_request.c -Wall -lnanomsg -lm -o ./dist/rr_request
gcc rr_proxy.c -Wall -lnanomsg -lm -o ./dist/rr_proxy
gcc rr_reply.c -Wall -lnanomsg -lm -o ./dist/rr_reply

frontend=tcp://127.0.0.1:5555
backend=tcp://127.0.0.1:5556


./dist/rr_proxy $frontend $backend & proxy=$! &&
./dist/rr_reply $backend & reply=$! &&
./dist/rr_request $frontend &&
# kill $request
kill $reply
kill $proxy


# ./dist/reqrep request ipc:///tmp/reqrep.ipc & request=$! && sleep 1
# ./dist/reqrep node1 ipc:///tmp/reqrep.ipc
# kill $request


# ./dist/reqrep request inproc://test & request=$! && sleep 1
# ./dist/reqrep node1 inproc://test
# kill $request