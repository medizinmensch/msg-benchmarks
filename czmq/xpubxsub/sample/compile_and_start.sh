
gcc rrclient.c -o ./dist/rrclient -lzmq
gcc rrworker.c -o ./dist/rrworker -L/usr/local/lib -lzmq
gcc rrbroker.c -o ./dist/rrbroker -L/usr/local/lib -lzmq



./dist/rrclient &
./dist/rrclient &
./dist/rrclient &
./dist/rrclient &
./dist/rrworker &
./dist/rrbroker