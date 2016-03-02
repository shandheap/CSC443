typedef struct Buffer { //Bookkeeping: keeps track of all necessary variables during external merge
    long rec_read; // number of record read so far from tape
    Record *buffer;
    long positionInOutputBuffer;
    long positionInInputBuffer;
} Buffer;

typedef struct heapRecord {
	int uid1;
	int uid2;
	int run_id;
} HeapRecord;

int runPhaseOne(FILE *inputFP, long totalRecords, long rec_per_sublist);
long getDivisible(long a, long b);
// base case: if the len of input_list <= one of the inputfile size

/** Phase 2**/
typedef struct TapeManager { //Bookkeeping: keeps track of all necessary variables during external merge
    HeapRecord *heap;
    int heapSize; // current heapsize
	int rec_per_sublist; //number of elements in each sublist
    int filenameFlag; // 0 means tape1 and tape2 are input, 1 means they are output
    long bufferCapacity; 
	Buffer *inputBuffers; 
	Buffer *outputBuffers;
    FILE **inputFPs;
    FILE **outputFPs;
} TapeManager;

int runPhaseTwo(TapeManager *manager);
