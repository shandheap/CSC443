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

   if (! (outputFile = fopen(filename, "a")) ) {
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
   HeapRecord * heap = (HeapRecord *) calloc(heapCapacity, sizeof(HeapRecord));

   // Go through all runs and fill heap
   for (int i = 0; i < heapCapacity; i++) {
      Record cur;
      getNextRecord(merger, i, &cur);
      insertIntoHeap(merger, i, &cur);
   }

   return 0;
}

int getNextRecord(MergeManager *merger, int run_id, Record *result) {
   return 0;
};

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

int insertIntoHeap (MergeManager *merger, int run_id, Record *newRecord) {
   HeapRecord * hrecord;
   hrecord->uid1 = newRecord->uid1;
   hrecord->uid2 = newRecord->uid2;
   hrecord->run_id = run_id;
   
   int child, parent;
   if (merger->heapSize == merger->heapCapacity) {
      printf( "Error: Heap is full\n");
      return -1;
   }
   
   child = merger->heapSize++; /* the next available slot in the heap */
   
   while (child > 0)    {
      parent = (child - 1) / 2;
      if (compare((void *) &(merger->heap[parent]), (void *) hrecord)>0) {
         merger->heap[child] = merger->heap[parent];
         child = parent;
      } else {
         break;
      }
   }
   
   merger->heap[child]= *hrecord;   
   
   return 0;
};

int getTopHeapElement (MergeManager *merger, int run_id, Record *result) {
   HeapRecord item;
   int child, parent;

   if (merger->heapSize == 0) {
      printf( "Error: Popping top element from an empty heap\n");
      return 1;
   }

   HeapRecord topElem = merger->heap[0];
   result->uid1 = topElem.uid1;
   result->uid2 = topElem.uid2;

   // now we need to reorganize heap - keep the smallest on top
   item = merger->heap [-- merger->heapSize]; // to be reinserted 

   parent = 0;

   do {
      child = (2 * parent) + 1;

      // if there are two children, compare them 
      if (child + 1 < merger->heapSize && (compare((void *)&(merger->heap[child]),(void *)&(merger->heap[child + 1]))>0)) {
         ++child;
      }
      // compare item with the larger 
      if (compare((void *)&item, (void *)&(merger->heap[child]))>0) {
         merger->heap[parent] = merger->heap[child];
         parent = child;
      } else {
         break;
      }
   } while (child < merger->heapSize);

   merger->heap[parent] = item;
   
   return 0;
};

int addToOutputBuffer(MergeManager *merger, Record * newRecord);

int flushOutputBuffer(MergeManager *merger);
