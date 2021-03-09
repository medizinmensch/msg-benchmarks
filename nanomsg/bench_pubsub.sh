gcc pubsub.c -lnanomsg -lm -o ./dist/pubsub
success=$?

if [[ success -eq 0 ]]
then
    ./dist/pubsub publisher ipc:///tmp/pubsub.ipc publisher0 & publisher0=$!
    ./dist/pubsub subscriber ipc:///tmp/pubsub.ipc subscriber0 & subscriber=$! && sleep 1
    # ./dist/pubsub publisher ipc:///tmp/pubsub.ipc publisher1 & publisher1=$!
    # ./dist/pubsub publisher ipc:///tmp/pubsub.ipc publisher2 & publisher2=$!
    sleep 3
    kill $subscriber $publisher0
    # kill $subscriber $publisher0 $publisher1 $publisher2
    
    rm /tmp/pubsub.ipc
fi

