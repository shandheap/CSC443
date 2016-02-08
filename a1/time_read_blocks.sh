#!/bin/bash
> read_blocks_timing.txt

sync
echo 3 > sudo /proc/sys/vm/drop_caches
./max_ave_seq_disk 1024 >> read_blocks_timing.txt

sync
echo 3 > sudo /proc/sys/vm/drop_caches
./max_ave_rand_disk 1024 >> read_blocks_timing.txt

sync
echo 3 > sudo /proc/sys/vm/drop_caches
./max_ave_seq_ram 1024 >> read_blocks_timing.txt

sync
echo 3 > sudo /proc/sys/vm/drop_caches
./max_ave_rand_ram 1024 >> read_blocks_timing.txt

python plot_read_blocks.py

