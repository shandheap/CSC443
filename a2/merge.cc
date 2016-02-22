#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "merge.h"


int makeRun (SortingManager manager) {
    Record * partitionBuffer = manager.partitionBuffer;
    FILE * inputFile = manager.inputFile;
    long totalRecords = manager.totalRecords;

    FILE * outputFile;
    if (! (outputFile = fopen(TEMP_FILE, "a")) ) {
        printf("Failed to open file %s\n", TEMP_FILE);
        return -1;
    }

    int size;
    size = fread(partitionBuffer, sizeof(Record), totalRecords, inputFile);
    if (size != totalRecords) return -1;
    
    qsort(partitionBuffer, totalRecords, sizeof(Record), compare);
    
    size = fwrite(partitionBuffer, sizeof(Record), totalRecords, outputFile);
    if (size != totalRecords) return -1;

    fclose(outputFile);

    return 0;
}
