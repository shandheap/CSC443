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

int mergeRuns (MergeManager *merger) {
    int  result;
    //1. go in the loop through all input files and fill-in initial buffers
    if (initInputBuffers(merger)!=0)
        return 1;
    
    //2. Initialize heap with 1 element from each buffer
    if (initHeap(merger)!=0)
        return 1;   
    
    while (merger->heapSize > 0) { //heap is not empty  
        Record smallest;
        Record next;
        
        int runID;
        
        if (getTopHeapElement (merger, &runID, &smallest)!=0)
            return 1;   

        merger->outputBuffer[merger->currentPositionInOutputBuffer++]=smallest;     

        result = getNextRecord (merger, runID, &next);

        if(next != NULL) {//next element exists     
            if(insertIntoHeap (merger, &next)!=0)
                return 1;
        }
        if(result==1) //error
            return 1;
        
        if(merger->currentPositionInOutputBuffer == merger-> outputBufferCapacity ) { //staying on the last slot of the output buffer - next will cause overflow
            if(flushOutputBuffer(merger)!=0)
                return 1;           
            merger->currentPositionInOutputBuffer=0;
        }   
    }
    //flush what remains in output buffer
    if(merger->currentPositionInOutputBuffer >0) { //there are elements in the output buffer    
        if(flushOutputBuffer(merger)!=0)
            return 1;
    }

    return 0;   
}

int initInputBuffers(MergeManager *merger) {
    int heapSize = merger->heapSize;
    InputBuffer * inputBuffers = (InputBuffer *) calloc(heapSize, sizeof(InputBuffer));

    int i;
    for (i=0; i<heapSize; i++) {
        InputBuffer curInput = inputBuffers[i];
        Record * buffer = (Record *) calloc(4, sizeof(Record));
        InputBuffer bufferInit = {
            4,
            10L,
            0L,
            0L,
            0,
            0,
            buffer
        };
        curInput = bufferInit;
    }

    merger->inputBuffers = inputBuffers;

    return 0;
};

int initHeap(MergeManager *merger);

int getNextRecord (MergeManager *merger, int run_id, Record *result);

int refillBuffer(MergeManager *merger, int run_id);

int insertIntoHeap (MergeManager *merger, Record *newRecord);

int getTopHeapElement (MergeManager *merger, int run_id, Record *result);

int addToOutputBuffer(MergeManager *merger, Record * newRecord);

int flushOutputBuffer(MergeManager *merger);

