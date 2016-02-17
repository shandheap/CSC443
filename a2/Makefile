CC = g++
all_progs = disk_sort

all: $(all_progs)

library.o: library.cc library.h
	$(CC) -o library.o -c library.cc

disk_sort: disk_sort.cc library.o
	$(CC) -o $@ $< library.o -g

clean:
	rm $(all_progs)
