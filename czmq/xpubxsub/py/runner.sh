python3 rrclient.py 1 &
python3 rrclient.py 2 &
python3 rrclient.py 3 &
python3 rrclient.py 4 &
python3 rrclient.py 5 &
python3 rrclient.py 6 &


python3 rrworker.py &

python3 rrbroker.py

# pkill -f "rrbroker" & pkill -f "rrclient" & pkill -f "rrworker"