#!/bin/bash

check_arguments () {
    if [ "$1" -ne 1 ]; then
        printf "compile.sh: Argument error. You need to pass 1 argument: target (zmq/nanomsg)"
        kill -s TERM $COMPILE_PID
    fi

    if [[ !"$target" == "zmq" && !"$target" == "nanomsg" ]]; then
        echo "compile.sh: Target was set to <$target> which is not valid. Valid options are 'nanomsg' and 'zmq'"
        kill -s TERM $COMPILE_PID
    fi
}

compile() {
    mkdir -p dist
    printf "Compiling"
    gcc ./xreqxrep/rr_uni_worker.c -o ./dist/rr_uni_worker -l$target -lm -D$target -O3 -march=native
    printf "."
    gcc ./xreqxrep/rr_uni_client.c -o ./dist/rr_uni_client -l$target -lm -D$target -O3 -march=native
    printf "."
    gcc ./xreqxrep/rr_${target}_broker.c -o ./dist/rr_$target_broker -l$target -lm -O3 -march=native
    printf ". done\n"
}

trap "exit 1" TERM
COMPILE_PID=$$

# MAIN
target=$1

check_arguments $#
compile
