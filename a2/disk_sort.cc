#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "library.h"
#include "merge.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <input_file> <total_mem> <block_size> <num_of_runs>\n", argv[0]);
        return -1;
    }

    FILE * inputFile;
    if (! (inputFile = fopen(argv[1], "r")) ) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }

    long totalMem = atol(argv[2]);
    long blockSize = atol(argv[3]);
    long totalPartitions = atol(argv[4]);
    long filesize = get_filesize(inputFile);
    long totalRecords = filesize / sizeof(Record);
    long partitionRecords = totalRecords / totalPartitions;
    long remRecords = totalRecords % totalPartitions;
    Record * partitionBuffer = (Record *) calloc(totalRecords, sizeof(Record));

    // Gracefully exit if partition is bigger than available memory
    if ((partitionRecords * sizeof(Record)) > totalMem) {
        printf("Partition size exceeded available memory, exiting program...\n");
        return -1;
    }


    // initialize sorting manager
    SortingManager manager = {
        partitionBuffer,
        inputFile,
        partitionRecords,
        totalPartitions
    };

    // sort one partition at a time
    long i;
    for (i=0; i<totalPartitions; i++) {
        if (makeRun(manager)) {
            printf("Failed to make run\n");
            return -1;
        }
    }

    // sort remaining records
    if (remRecords) {
        manager.totalRecords = remRecords;
        if (makeRun(manager)) {
            printf("Failed to make run\n");
            return -1;
        }
    }

    fclose(inputFile);
    free(partitionBuffer);

    return 0;
}
