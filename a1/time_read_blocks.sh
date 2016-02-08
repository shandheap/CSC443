#!/bin/bash
> read_blocks_timing.txt

./max_ave_seq_disk 1024 >> read_blocks_timing.txt

./max_ave_seq_ram 1024 >> read_blocks_timing.txt

python plot_read_blocks.py

