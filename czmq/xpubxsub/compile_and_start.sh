
# gcc rrclient.c -o ./dist/rrclient -L/usr/local/lib -lzmq -lm
# # gcc rrworker.c -o ./dist/rrworker -L/usr/local/lib -lzmq
# # gcc rrbroker.c -o ./dist/rrbroker -L/usr/local/lib -lzmq



# # ./dist/rrclient &
# # ./dist/rrworker &
# # ./dist/rrbroker

gcc rrclient.c -o ./dist/rrclient -lzmq -lm
gcc rrworker.c -o ./dist/rrworker -lzmq -lm
gcc rrbroker.c -o ./dist/rrbroker -lzmq -lm



./dist/rrclient 1 &
# ./dist/rrclient &
# ./dist/rrclient &
# ./dist/rrclient &
./dist/rrworker &
./dist/rrbroker
