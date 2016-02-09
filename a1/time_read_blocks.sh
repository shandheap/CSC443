#!/bin/bash
> read_blocks_timing.txt

./max_ave_seq_disk edges.dat 1024 >> read_blocks_timing.txt

./max_ave_seq_ram edges.dat 1024 >> read_blocks_timing.txt

python plot_read_blocks.py

