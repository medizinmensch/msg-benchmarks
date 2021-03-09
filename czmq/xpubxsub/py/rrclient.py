#
#   Request-reply client in Python
#   Connects REQ socket to tcp://localhost:5559
#   Sends "Hello" to server, expects "World" back
#
import sys
import zmq

client = sys.argv[1]
print(f"Starting Client No. {client}")
#  Prepare our context and sockets
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5559")

#  Do 10 requests, waiting each time for a response
for request in range(1,500):
    socket.send_string(f"{client}")
    message = socket.recv()
    print("Received reply %s [%s]" % (request, message))

socket.send_string(f"FINALMSG{client}")
socket.close()
context.term()

print(f"Clsoing client {client}")