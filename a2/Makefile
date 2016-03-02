CC = g++
all_progs = disk_sort max_average tape_sort

all: $(all_progs)

library.o: library.cc library.h
	$(CC) -o library.o -c library.cc

merge.o: merge.cc merge.h
	$(CC) -o merge.o -c merge.cc

tape_merge.o: tape_merge.cc tape_merge.h
	$(CC) -o tape_merge.o -c tape_merge.cc
	
max_average: max_average.cc library.o
	$(CC) -o $@ $< library.o -g

disk_sort: disk_sort.cc library.o merge.o
	$(CC) -o $@ $< library.o merge.o -g
	
tape_sort: tape_sort.cc library.o tape_merge.o
	$(CC) -o $@ $< library.o tape_merge.o -g

clean:
	rm $(all_progs)
