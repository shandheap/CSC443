#!/bin/bash

for size in 512 1024 4096 8192 16384 32768 1048576 2097152 4194304 6291456 8388608 10485760 12055168
do
  sync
  echo 3 > sudo /proc/sys/vm/drop_caches
  echo $size: `./write_blocks g_plusAnonymized.csv $size`
done
