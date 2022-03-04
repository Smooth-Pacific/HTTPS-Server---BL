#!/bin/bash


for (( i=1; i<20; i++ ))
do
	curl -s --digest --user myuser:mypass 'https://localhost:8080' -w "%{time_total}\n" >> temp.txt
done

lat=$(awk "/[0-9]/" temp.txt | awk ' { for (i=1;i<=NF  ;i++){ sum+=$i;num++} } END{ print(sum/num) } ')

echo Avg Latency for 20 requests: $lat seconds

rm temp.txt