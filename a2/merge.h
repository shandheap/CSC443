/**
 * Phase 1. Sorting of records in each partition
 * **/
typedef struct SortingManager {
    Record * partitionBuffer; //working buffer to be sorted 
    FILE * inputFile; //pointer to the input file - opened and closed only once
    long totalRecords; //total records in current partition 
    long totalPartitions;
} SortingManager;


int makeRun (SortingManager manager); //sorts each partition in turn, writes it to a temp disk file, returns zero on success

/**
 * Phase 2. Multiway merge 
 * **/
typedef struct InputBuffer { //Input buffer - responsible for a single run
    int capacity; //how many elements max can it hold
    long runLength; //total length of the corresponding run
    long currPositionInFile; //where to read next
    long currentBufferPosition; //where to read the next element of the buffer
    long totalElements; //how many total elements in buffer - may be different from max capacity
    int done; //set to 1 when the run is finished
    Record *buffer;  //array of Recordords
} InputBuffer;

typedef struct MergeManager { //Bookkeeping: keeps track of all necessary variables during external merge
    Record *heap; //keeps 1 from each buffer in top-down order - smallest on top (according to compare function)
        int heapSize; //current number of elements in the heapSizep
    int heapCapacity; //max number of elements
    FILE *inputFP; //stays comparelosed, opens each time we need to refill some amount of data from a disk run
    FILE *outputFP;  //stays open to flush output buffer when full
    Record * outputBuffer; //buffer to store output elements until they are flushed to disk
    int currentPositionInOutputBuffer;  //where to add next element in output buffer
    int outputBuffertBufferCapacity; //how many elements max can it hold
    InputBuffer *InputBuffertBuffers; //array of buffers to buffer separate runs
} MergeManager;

int mergeRuns (MergeManager *merger); //merges all runs into a single sorted list
int initInputBuffers(MergeManager *merger); //initial fill of input buffers with elements of each run
int initHeap(MergeManager *merger); //inserts into heap one element from each buffer - to keep the smallest on top
int getNextRecord (MergeManager *merger, int run_id, Record *result); //reads the next element from an input buffer, 
// uploads next part of a run from disk if necessary by calling refillBuffer
int refillBuffer(MergeManager *merger, int run_id);
int insertIntoHeap (MergeManager *merger, Record *newRecord); //inserts next element into heap
int getTopHeapElement (MergeManager *merger, int run_id, Record *result); //removes smallest element from the heap, and restores heap order
int addToOutputBuffer(MergeManager *merger, Record * newRecord); //adds next smallest element to the output buffer, flushes buffer if full by calling flushOutputBuffer
int flushOutputBuffer(MergeManager *merger);
