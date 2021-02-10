#!/usr/bin/env bash

base_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

czmq/czmq_benchmark.sh
nanomsg/nanomsg_benchmark.sh