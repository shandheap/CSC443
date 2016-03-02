#include <stdlib.h>
#include <stdio.h>
#include "library.h"
#include "tape_merge.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <total_mem>\n", argv[0]);
        return -1;
    }
    
    FILE * inputFP;
    if (! (inputFP = fopen(argv[1], "r")) ) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }
    long totalMem = atol(argv[2]);
    long filesize = get_filesize(inputFP);
    long totalRecords = filesize / sizeof(Record);  
    // TODO: assuming this is can divide totalRecords. If not, decrease it's value until it is divisible
    long rec_per_sublist = getDivisible(totalRecords, totalMem / sizeof(Record)); 
    
    /** Phase 1**/
    runPhaseOne(inputFP, totalRecords, rec_per_sublist);
    
    /** Phase 2**/
    Buffer *inputBuffers = (Buffer *) calloc(2, sizeof(Buffer));
    Buffer *outputBuffers = (Buffer *) calloc(2, sizeof(Buffer));
    long bufferCapacity = totalMem / 4;
    HeapRecord *heap = (HeapRecord *) calloc(2, sizeof(HeapRecord));
    FILE ** inputFPS = (FILE **) calloc(2, sizeof(FILE*));
    FILE ** outputFPS = (FILE **) calloc(2, sizeof(FILE*));
    TapeManager tapeManager = {
        heap,
        0,
        rec_per_sublist,
        0,
        bufferCapacity, 
        inputBuffers,
        outputBuffers,
        inputFPS,
        outputFPS,
    }; 
    
    runPhaseTwo(&tapeManager);
    
    return 0;
}
