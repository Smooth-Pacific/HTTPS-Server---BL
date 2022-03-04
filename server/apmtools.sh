#!/bin/bash

mkdir apmlogs

mpstat > apmlogs/mpstat.txt
vmstat > apmlogs/vmstat.txt
top -b -n1 > apmlogs/top.txt 
free > apmlogs/free.txt
iostat > apmlogs/iostat.txt

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=apmlogs/valgrind-out.txt ./server &
PID=$!
sleep 10
kill $PID

mpstat >> apmlogs/mpstat.txt
vmstat >> apmlogs/vmstat.txt
top -b -n1 >> apmlogs/top.txt 
free >> apmlogs/free.txt
iostat >> apmlogs/iostat.txt