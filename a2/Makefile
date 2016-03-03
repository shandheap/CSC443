CC = g++
all_progs = disk_sort max_ave_followers

all: $(all_progs)

library.o: library.cc library.h
	$(CC) -o library.o -c library.cc

merge.o: merge.cc merge.h
	$(CC) -o merge.o -c merge.cc

max_ave_followers: max_ave_followers.cc library.o
	$(CC) -o $@ $< library.o

disk_sort: disk_sort.cc library.o merge.o
	$(CC) -o $@ $< library.o merge.o

clean:
	rm $(all_progs) *.o
