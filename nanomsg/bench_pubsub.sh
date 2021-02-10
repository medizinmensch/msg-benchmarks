gcc pubsub.c -lnanomsg -o ./dist/pubsub

./dist/pubsub server ipc:///tmp/pubsub.ipc & server=$! && sleep 1
./dist/pubsub client ipc:///tmp/pubsub.ipc client0 & client0=$!
./dist/pubsub client ipc:///tmp/pubsub.ipc client1 & client1=$!
./dist/pubsub client ipc:///tmp/pubsub.ipc client2 & client2=$!
sleep 5
kill $server $client0 $client1 $client2