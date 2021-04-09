# ZMQ vs nanomsg Benchmarking for XREQ / XREP

Benchmarking of C-Libraries for both ZMQ (`czmq`) as well as nanomsg for a XREQ/XREP pattern.

Similar to a XPUB/XSUB pattern which utilizes multiple publisher and subscriber nodes, this XREQ/XREP has multiple processes sending messages to a single master process via a router/proxy process. 


