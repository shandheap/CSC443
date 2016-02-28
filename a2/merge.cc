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
/*
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
*/
int initInputBuffers(MergeManager *merger) {
    int heapSize = merger->heapSize;
    InputBuffer * inputBuffers = (InputBuffer *) calloc(heapSize, sizeof(InputBuffer));
    for (int i=0; i<heapSize; i++) {
        int max_size = merger->outputBufferCapacity;
        Record * buffer = (Record *) calloc(max_size, sizeof(Record));
        InputBuffer bufferInit = {
            max_size,
            0L, /* This will be reinitialised in refillBuffer */
            0L,
            0L,
            0,
            0,
            buffer
        };
        inputBuffers[i] = bufferInit;
    }

    merger->inputBuffers = inputBuffers;
    
    // now fills the input buffers with actual values from disk temp.dat files
    for (int i=0; i<heapSize; i++) {
        refillBuffer(merger, i);
    }

    return 0;
};

int initHeap(MergeManager *merger);

int getNextRecord (MergeManager *merger, int run_id, Record *result);

int refillBuffer(MergeManager *merger, int run_id) {
    
    InputBuffer *bufferToRefill = &merger->inputBuffers[run_id];
    
    // Name of the temp file corresponding to the run_id
    char filename[17];
    FILE *inputFile;
    sprintf(filename, "temp%d.dat", run_id);
    if (! (inputFile = fopen(filename, "r")) ) {
        printf("Failed to open file %s\n", filename);
        return -1;
    }    
    // Reset the currentBufferPosition to 0
    bufferToRefill->currentBufferPosition = 0L;
    
    // initialise the runLength of the input buffer if it is not done yet
    if (bufferToRefill->runLength == 0) {
        bufferToRefill->runLength = get_filesize(inputFile) / sizeof(Record);
    }
    // Check if the run is depleted already
    if (bufferToRefill->currPositionInFile >= bufferToRefill->runLength * (long) sizeof(Record)) {
        return -1;
    }
    // Read records drom disk and fill the inputBuffer with them
    int rec_rem_to_read =  bufferToRefill->runLength - (int) (bufferToRefill->currPositionInFile / sizeof(Record));
    int rec_to_read = (bufferToRefill->capacity > rec_rem_to_read) ? rec_rem_to_read : bufferToRefill->capacity;
    Record *buffer = (Record *) calloc(rec_to_read, sizeof(Record));
    fseek(inputFile, bufferToRefill->currPositionInFile, SEEK_SET); // Set the filepointer to bufferToRefill->currPositionInFile
    fread(buffer, sizeof(Record), rec_to_read, inputFile);
    bufferToRefill->buffer = buffer;
    
    // Shift the bufferToRefill->currPositionInFile
    bufferToRefill->currPositionInFile += rec_to_read * sizeof(Record);
    
    fclose(inputFile);
    
    return 0;
}

int insertIntoHeap (MergeManager *merger, Record *newRecord);

int getTopHeapElement (MergeManager *merger, int run_id, Record *result);

int addToOutputBuffer(MergeManager *merger, Record * newRecord);

int flushOutputBuffer(MergeManager *merger);

