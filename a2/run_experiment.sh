#!/bin/bash

time=/usr/bin/time
total_mem=209715200
block_size=8192

if [ $# -ne 1 ]; then
    echo "Usage: $0 <number-of-runs>"
else
    k=$1
    for i in 1 2 4 16; do
        num_runs=$(( k / i ))
        fn=time_sort_$i.txt

        $time -v disk_sort edges.dat $total_mem $block_size $num_runs &> $fn
    done
fi
