CC = g++
all_progs = disk_sort max_average

all: $(all_progs)

library.o: library.cc library.h
	$(CC) -o library.o -c library.cc

merge.o: merge.cc merge.h
	$(CC) -o merge.o -c merge.cc

max_average: max_average.cc library.o
	$(CC) -o $@ $< library.o -g

disk_sort: disk_sort.cc library.o merge.o
	$(CC) -o $@ $< library.o merge.o -g

clean:
	rm $(all_progs)
