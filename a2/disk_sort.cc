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
    partitionRecords += remRecords;
    Record * partitionBuffer = (Record *) calloc(partitionRecords, sizeof(Record));

    // Gracefully exit if partition is bigger than available memory
    if ((partitionRecords * sizeof(Record)) > totalMem) {
        printf("Partition size exceeded available memory, exiting program...\n");
        return -1;
    }

    /* Phase 1 */

    // initialize sorting manager
    SortingManager sortingManager = {
        partitionBuffer,
        inputFile,
        partitionRecords,
        totalPartitions
    };

    // sort one partition at a time
    long i;
    for (i=0; i<totalPartitions; i++) {
        if (makeRun(sortingManager, i)) {
            printf("Failed to make run\n");
            return -1;
        }
        // change number of records to read after first bigger chunk
        if (i == 0) sortingManager.totalRecords -= remRecords;
    }

    fclose(inputFile);
    free(partitionBuffer);

    /* Phase 2 
    FILE * outputFile;
    if (! (outputFile = fopen(TEMP_FILE, "r")) ) {
        printf("Failed to open sorted file to write to\n");
        return -1;
    }

    blockSize -= blockSize % sizeof(Record);
    int blockRecords = blockSize / sizeof(Record);

    Record * heap = (Record *) calloc(totalPartitions, sizeof(Record));
    Record * outputBuffer = (Record *) calloc(blockRecords, sizeof(Record));
    MergeManager mergeManager = {
        heap,
        0,
        totalPartitions,
        0,
        outputFile,
        outputBuffer,
        0,
        blockRecords,
        0
    };

    free(heap);
    fclose(inputFile);
    fclose(outputFile);
    */

    return 0;
}
