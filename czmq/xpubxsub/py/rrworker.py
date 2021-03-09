#
#   Request-reply service in Python
#   Connects REP socket to tcp://localhost:5560
#   Expects "Hello" from client, replies with "World"
#
import zmq

print("Starting Worker")

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.connect("tcp://localhost:5560")

# active_clients = {'1','2','3','4','5','6'}
active_clients = set()

while True:
    message = socket.recv().decode("ascii")
    print("Received request: %s" % message)
    
    if message.startswith('FINALMSG'):
        client = message[8:]
        active_clients.discard(client)
        print(f"active_clients: {active_clients}")
        if len(active_clients) == 0:
            print("WORKER: BREAKING NOW")
            break
    else:
        active_clients.add(message)
    
    socket.send_string("World")

socket.close()
context.term()