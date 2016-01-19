CC = g++

all: write_blocks max_ave_seq_disk ...
library.o: library.cc library.h
	$(CC) -o library.o -c library.cc

write_blocks: write_blocks.cc library.o
	$(CC) -o $@ $< library.o -g

max_ave_seq_disk: max_ave_seq_disk.cc library.o
	$(CC) -o $@ $< library.o -g
