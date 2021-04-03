#!/usr/bin/env bash

script_dirb=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

clients=$1

gcc rr_request.c -Wall -lnanomsg -lm -o ./dist/rr_request
gcc rr_proxy.c -Wall -lnanomsg -lm -o ./dist/rr_proxy
gcc rr_reply.c -Wall -lnanomsg -lm -o ./dist/rr_reply

frontend=tcp://127.0.0.1:5555
backend=tcp://127.0.0.1:5556

./dist/rr_proxy $backend $frontend & proxy=$! &&
./dist/rr_reply $backend & reply=$! &&


for (( i=1; i<=$clients; i++ ))
do
   printf "$i "
   ./dist/rr_request $frontend
done



# kill $request
# printf "killing reply: $reply\n"
# kill $reply
# printf "killing proxy: $proxy\n"
# kill $proxy


# ./dist/reqrep request ipc:///tmp/reqrep.ipc & request=$! && sleep 1
# ./dist/reqrep node1 ipc:///tmp/reqrep.ipc
# kill $request


# ./dist/reqrep request inproc://test & request=$! && sleep 1
# ./dist/reqrep node1 inproc://test
# kill $request

printf "\n\n---------Done---------\n"
pkill -f "rr_reply" & pkill -f "rr_request" & pkill -f "rr_proxy"
