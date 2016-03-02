#!/bin/bash

time=/usr/bin/time
total_mem=209715200
block_size=8192
num_runs=8

$time -v sort -t "," -n -k2 edges.csv > edges_sorted_uid2.csv &> unix_sort_time.txt

$time -v disk_sort edges.dat $total_mem $block_size $num_runs &> disk_sort_time.txt
