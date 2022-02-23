#!/bin/bash


./server &

out=$(curl -XGET --digest --user myuser:mypass -k 'https://localhost:8080')

VAR="SUCCESS, Hello World!"

if [ "$out" = "$VAR" ]; then
    echo "Output as expected"
else
    echo "Error, output not expected"
fi

ps -ef | grep server | grep -v grep | awk '{print $2}' | xargs kill