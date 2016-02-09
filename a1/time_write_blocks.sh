#!/bin/bash

# Start with empty output file
> write_blocks_timing.txt

for size in 512 1024 4096 8192 16384 32768 1048576 2097152 4194304 8388608
do
  sync
  sudo bash -c "echo 1 > /proc/sys/vm/drop_caches"
  result=`./write_blocks edges.csv $size`
  echo $result >> write_blocks_timing.txt
done

python plot_write_blocks.py
