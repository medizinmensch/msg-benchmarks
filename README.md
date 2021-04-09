# ZMQ vs nanomsg Benchmarking for XREQ / XREP

Benchmarking of C-Libraries for both ZMQ (`czmq`) as well as nanomsg for a XREQ/XREP pattern.

Similar to a XPUB/XSUB pattern which utilizes multiple publisher and subscriber nodes, this XREQ/XREP has multiple processes sending messages to a single master process via a router/proxy process. 

# How to run (Unix)

Make sure have the czmq and/or the nanomsg library installed. 


## Compilation
Compiles czmq or nanomsg and saves in the `./dist` folder. Example:
```bash
./compile.sh czmq|nanomsg
```


## Running
Runs proxy, master and a number of clients with a url (tcp (both) or ipc(nanomsg)) for a msg size up to 2^x. Example:
```bash
./run.sh frontend_url backend_url amount_of_clients biggest_msg_size_exponent czmq|nanomsg
# Exanmple:
./run.sh "tcp://localhost:5559" "tcp://localhost:5560" 1 20 czmq
```

## Benchmark

You need 4 Arguments:
1. target (czmq or nanomsg) 
2. Protocoll (ipc or tcp)
3. maximum amount of clients as power (base is 2)
4. The power of the biggest msg size you want to send (base is 2)\n"
        printf 'A good starting point would me: \n\t: ./benchmark.sh czmq ipc 4 16\n\t-> Results in a max msg size of 64KiB and 128 clients\n'
        printf 'Note that this cmq implementation does not does not with IPC.\n\n'

Example: 
```bash
./benchmark.sh czmq ipc 4 16
```
