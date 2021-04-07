#!/bin/bash

gcc reqrep.c -lnanomsg -o rr_reqrep


url=$1
#tcp://127.0.0.1:5559 # ipc:///tmp/reqrep.ipc
#url=ipc:///tmp/reqrep.ipc
frontend=tcp
./rr_reqrep node0 $url & node0=$!
./rr_reqrep node1 $url
kill $node0