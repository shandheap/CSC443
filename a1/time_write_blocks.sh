#!/bin/bash

# Start with empty output file
> write_blocks_timing.txt

for size in 512 1024 4096 8192 16384 32768 1048576 2097152 4194304
do
  result=`./write_blocks g_plusAnonymized.csv $size`
  echo $result >> write_blocks_timing.txt
done

python plot_write_blocks.py
