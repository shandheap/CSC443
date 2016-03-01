#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "merge.h"


int makeRun (SortingManager manager, int run_id) {
    Record * partitionBuffer = manager.partitionBuffer;
    FILE * inputFile = manager.inputFile;
    long totalRecords = manager.totalRecords;

    FILE * outputFile;
    char filename[17];
    snprintf(filename, sizeof(filename), "temp%d.dat", run_id);

    if (! (outputFile = fopen(filename, "w")) ) {
        printf("Failed to open file %s\n", filename);
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
    int result;

    //1. go in the loop through all input files and fill-in initial buffers
    if (initInputBuffers(merger))
        return 1;
     
    //2. Initialize heap with 1 element from each buffer
    if (initHeap(merger))
        return 1;

    while (merger->heapSize > 0) { //heap is not empty  
        HeapRecord smallest;
  
        if (getTopHeapElement (merger, &smallest))
            return 1; 

        Record r;
        r.uid1 = smallest.uid1;
        r.uid2 = smallest.uid2;
        int run_id = smallest.run_id;

        merger->outputBuffer[merger->currentPositionInOutputBuffer++] = r;     

        Record next;
        result = getNextRecord (merger, run_id, &next);

        if (result == 0) {//next element exists     
            if (insertIntoHeap (merger, run_id, &next))
                return 1;
        }
        
        if (result == 1) //error
            return 1;

        // staying on the last slot of the output buffer - next will cause overflow
        if (merger->currentPositionInOutputBuffer == merger->outputBufferCapacity) { 
            if (flushOutputBuffer(merger))
                return 1;           
        } 
    }

    // flush what remains in output buffer
    if (merger->currentPositionInOutputBuffer) { // there are elements in the output buffer    
        if (flushOutputBuffer(merger))
            return 1;
    }

    return 0;
}

int initInputBuffers(MergeManager *merger) {
    int heapCapacity = merger->heapCapacity;
    InputBuffer * inputBuffers = (InputBuffer *) calloc(heapCapacity, sizeof(InputBuffer));
    for (int i=0; i<heapCapacity; i++) {
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
    for (int i=0; i<heapCapacity; i++) {
        refillBuffer(merger, i);
    }

    return 0;
};

int initHeap(MergeManager *merger) {
    int heapCapacity = merger->heapCapacity;

    // Initialize the heap
    merger->heap = (HeapRecord *) calloc(heapCapacity, sizeof(HeapRecord));

    // Go through all runs and fill heap
    for (int i = 0; i < heapCapacity; i++) {
        Record cur;
        getNextRecord(merger, i, &cur);
        insertIntoHeap(merger, i, &cur);
    }

    return 0;
}

int getNextRecord(MergeManager *merger, int run_id, Record *result) {   
    InputBuffer * inputBuffer = &(merger->inputBuffers[run_id]);

    // Check if there is any more records left get from the buffer
    if (inputBuffer->currentBufferPosition >= inputBuffer->totalElements) {
        int status = refillBuffer(merger, run_id);
        if (status != 0) { // The run is depleted or there was some error
            return status;
        }
    }
    *result = inputBuffer->buffer[inputBuffer->currentBufferPosition ++];
   
    return 0;
}

int refillBuffer(MergeManager *merger, int run_id) {
    InputBuffer *bufferToRefill = &merger->inputBuffers[run_id];
 
    // Name of the temp file corresponding to the run_id
    char filename[17];
    FILE *inputFile;
    sprintf(filename, "temp%d.dat", run_id);
    if (! (inputFile = fopen(filename, "r")) ) {
        printf("Failed to open file %s\n", filename);
        return 1;
    }
    // initialise the runLength of the input buffer if it is not done yet
    if (bufferToRefill->runLength == 0) {
        bufferToRefill->runLength = get_filesize(inputFile) / sizeof(Record);
    }
    // initialise the capacity of the input buffer if it is not done yet
    if (bufferToRefill->capacity == 0) {
        bufferToRefill->capacity = merger->outputBufferCapacity;
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
    // Set the total element in bufferToRefill
    bufferToRefill->totalElements = (long) rec_to_read;
    // Reset the currentBufferPosition to 0
    bufferToRefill->currentBufferPosition = 0L;
    // Shift the bufferToRefill->currPositionInFile
    bufferToRefill->currPositionInFile += rec_to_read * sizeof(Record);

    fclose(inputFile);

    return 0;
}

int insertIntoHeap (MergeManager *merger, int run_id, Record *newRecord) {
    HeapRecord hrecord;
    hrecord.uid1 = newRecord->uid1;
    hrecord.uid2 = newRecord->uid2;
    hrecord.run_id = run_id;

    int child, parent;
    if (merger->heapSize == merger->heapCapacity) {
        printf( "Error: Heap is full\n");
        return -1;
    }

    child = merger->heapSize++; /* the next available slot in the heap */

    while (child > 0)    {
        parent = (child - 1) / 2;
        if (compare((void *) &(merger->heap[parent]), (void *) &hrecord)>0) {
            merger->heap[child] = merger->heap[parent];
            child = parent;
        } else {
            break;
        }
    }

    merger->heap[child]= hrecord;   

    return 0;
};

int getTopHeapElement (MergeManager *merger, HeapRecord *result) {
    HeapRecord item;
    int child, parent;

    if (merger->heapSize == 0) {
        printf( "Error: Popping top element from an empty heap\n");
        return -1;
    }

    HeapRecord topElem = merger->heap[0];
    result->uid1 = topElem.uid1;
    result->uid2 = topElem.uid2;
    result->run_id = topElem.run_id;
    // now we need to reorganize heap - keep the smallest on top
    item = merger->heap [-- merger->heapSize]; // to be reinserted 

    parent = 0;

    while ((child = (2 * parent) + 1) < merger->heapSize) {
        // if there are two children, compare them 
        if (child + 1 < merger->heapSize && (compare((void *)&(merger->heap[child]),(void *)&(merger->heap[child + 1]))>0)) {
	    ++child;
        }
	
        // compare item with the larger 
        if (compare((void *)&item, (void *)&(merger->heap[child]))>0) {
            merger->heap[parent] = merger->heap[child];
            parent = child;
        }
        else {
            break;
        }
    }

    merger->heap[parent] = item;

    return 0;
};

int addToOutputBuffer(MergeManager *merger, Record * newRecord) {
    int currentPositionInOutputBuffer = merger->currentPositionInOutputBuffer;
    int outputBufferCapacity = merger->outputBufferCapacity;

    // Check if buffer is full and flush its contents
    bool is_full = currentPositionInOutputBuffer == outputBufferCapacity;
    if (is_full && flushOutputBuffer(merger)) {
        printf("Error: Failed to flush the output buffer\n");
        return -1;
    }

    // Add new record to output buffer
    merger->outputBuffer[currentPositionInOutputBuffer++] = *newRecord;

    return 0;
}

int flushOutputBuffer(MergeManager *merger) {
    Record * outputBuffer = merger->outputBuffer;
    FILE * outputFP = merger->outputFP;
    int size;
    size = fwrite(outputBuffer, sizeof(Record), merger->currentPositionInOutputBuffer, outputFP);
    if (size != merger->currentPositionInOutputBuffer) return -1;
    merger->currentPositionInOutputBuffer = 0;  
    
    return 0;
}
