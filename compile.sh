#!/bin/bash

function compile_czmq {
    mkdir -p dist
    printf "Compiling."
    gcc ./xreqxrep/rr_uni_client.c -o ./dist/rr_uni_client -lzmq -lm -Dczmq -O3 -march=native
    printf "."
    gcc ./xreqxrep/rr_uni_worker.c -o ./dist/rr_uni_worker -lzmq -lm -Dczmq -O3 -march=native
    printf "."
    gcc ./xreqxrep/rr_zmq_broker.c -o ./dist/rr_zmq_broker -lzmq -lm -O3 -march=native
    printf "done\n"
}

function compile_nanomsg {
    mkdir -p dist
    printf "Compiling."
    gcc ./xreqxrep/rr_uni_client.c -o ./dist/rr_uni_client -lnanomsg -lm -Dnanomsg -O3 -march=native
    printf "."
    gcc ./xreqxrep/rr_uni_worker.c -o ./dist/rr_uni_worker -lnanomsg -lm -Dnanomsg -O3 -march=native
    printf "."
    gcc ./xreqxrep/rr_nanomsg_broker.c -o ./dist/rr_nanomsg_broker -lnanomsg -lm -O3 -march=native
    printf "done\n"
}

# trap cleanup EXIT
function cleanup {
  pkill -f "rr_.*"
  printf "\nCleanup OK\n"
}
trap cleanup INT


# MAIN
target=$1

if [[ "$target" = "czmq" ]]; then
  printf "Compiling for ${target}. "
  compile_czmq
elif [[ "$target" = "nanomsg" ]]; then
  printf "Compiling for ${target}. "
  compile_nanomsg
else
  echo "Compile.sh: Target was set to <$target> which is not valid. Valid options are 'nanomsg' and 'czmq'"
fi
