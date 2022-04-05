#!/bin/bash

# Create directory for Data Throughput Monitoring
dir=data_throughput
if [[ -e $dir ]]; then
	cd $dir
else
	mkdir $dir
	cd $dir
fi

wt="write_throughput.txt"
wi="write_iops.txt"
rt="read_throughput.txt"
ri="read_iops.txt"
ping="ping.txt"
summary="summary"

#Test write throughput by performing sequential writes with 8 parallel streams, using an I/O block size of 1 MB and an I/O depth of 64
fio --name=write_throughput --numjobs=8 --size=1G --time_based --runtime=60s --ramp_time=2s --ioengine=libaio --direct=1 --verify=0 --bs=1M --iodepth=64 --rw=write --group_reporting=1 --output="$wt"
#Test write IOPS by performing random writes, using an I/O block size of 4 KB and an I/O depth of 64
fio --name=write_iops --size=1G --time_based --runtime=60s --ramp_time=2s --ioengine=libaio --direct=1 --verify=0 --bs=4K --iodepth=64 --rw=randwrite --group_reporting=1 --output="$wi"
#Test read throughput by performing sequential reads with 8 parallel streams, using an I/O block size of 1 MB and an I/O depth of 64
fio --name=read_throughput --numjobs=8 --size=1G --time_based --runtime=60s --ramp_time=2s --ioengine=libaio --direct=1 --verify=0 --bs=1M --iodepth=64 --rw=read --group_reporting=1 --output="$rt"
#Test read IOPS by performing random reads, using an I/O block size of 4 KB and an I/O depth of 64
fio --name=read_iops --size=1G --time_based --runtime=60s --ramp_time=2s --ioengine=libaio --direct=1 --verify=0 --bs=4K --iodepth=64 --rw=randread --group_reporting=1 --output="$ri"
#Test I/O latency with ioping
ioping -c 100 . > "$ping"

# Output stats in to summary file
cat "$wt" >> "$summary"
echo "" >> "$summary"
cat "$wi" >> "$summary"
echo "" >> "$summary"

cat "$rt" >> "$summary"
echo "" >> "$summary"
cat "$ri" >> "$summary"
echo "" >> "$summary"

tail -n 4 "$ping" >> "$summary"