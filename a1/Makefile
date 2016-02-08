CC = g++
all_progs = write_blocks write_lines max_ave_seq_disk max_ave_seq_ram max_ave_rand_disk max_ave_rand_ram

all: $(all_progs) 
library.o: library.cc library.h
	$(CC) -o library.o -c library.cc

write_blocks: write_blocks.cc library.o
	$(CC) -o $@ $< library.o -g

write_lines: write_lines.cc library.o
	$(CC) -o $@ $< library.o -g	

max_ave_seq_disk: max_ave_seq_disk.cc library.o
	$(CC) -o $@ $< library.o -g
	
max_ave_seq_ram: max_ave_seq_ram.cc library.o
	$(CC) -o $@ $< library.o -g

max_ave_rand_disk: max_ave_rand_disk.cc library.o
	$(CC) -o $@ $< library.o -g

max_ave_rand_ram: max_ave_rand_ram.cc library.o
	$(CC) -o $@ $< library.o -g

clean:
	rm $(all_progs)
