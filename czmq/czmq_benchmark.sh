#!/usr/bin/env bash

script_dira=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

gcc req.c -lczmq -lm -o ./dist/req
gcc rep.c -lczmq -o ./dist/rep

./dist/rep "tcp://127.0.0.1:5555" &
./dist/rep "tcp://127.0.0.1:5555"

# ./dist/rep "ipc:///tmp/feeds/0" &
# ./dist/req "ipc:///tmp/feeds/0"

