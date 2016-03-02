#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "tape_merge.h"

int runPhaseOne(FILE * inputFP, long totalRecords, long rec_per_sublist) {
    long totalPartitions = totalRecords / rec_per_sublist;
    Record *buffer = (Record *) calloc(rec_per_sublist, sizeof(Record));
    FILE *ouputFPOne, *ouputFPTwo;
    
    if (! (ouputFPOne = fopen("tape1.dat", "w")) ) {
        printf("Failed to open file tape1.dat\n");
        return -1;
    }
    if (! (ouputFPTwo = fopen("tape2.dat", "w")) ) {
        printf("Failed to open file tape2.dat\n");
        return -1;
    }
    
    for (int i = 0; i < totalPartitions; i++) {
        fread(buffer, sizeof(Record), rec_per_sublist, inputFP);
        // sort this partition
        qsort(buffer, rec_per_sublist, sizeof(Record), compare);
        // write to the output files alternatively
        int size;
        if (i%2 == 0) {
            size = fwrite(buffer, sizeof(Record), rec_per_sublist, ouputFPOne);
        }
        else {
            int size = fwrite(buffer, sizeof(Record), rec_per_sublist, ouputFPTwo);
        }
        // check if the data was written to disk properly
        if (size != rec_per_sublist) {
            printf("Error in phase 1\n");
            return -1;
        }
    }
    free(buffer);
    fclose(inputFP);
    fclose(ouputFPOne);
    fclose(ouputFPTwo);
    
    return 0;
}

/** Return maximum possible int n such that n <= b and a mod n equals zero**/ 
long getDivisible(long a, long b) {
    while(1) {
        if (a % b == 0) {
            return b;
            break;
        }
        b--;
    }
}

int refillBuffer(TapeManager *manager, int id) {
    if (manager->inputBuffers[id].rec_read <= manager->rec_per_sublist) {
        return 1; // No more to refill for this sublist
    }
    int rec_to_read = manager->bufferCapacity - manager->inputBuffers[id].rec_read;
    // check if u r reading past the sublist
    rec_to_read = (rec_to_read > manager->rec_per_sublist) ? manager->rec_per_sublist : rec_to_read;
    fread(&manager->inputBuffers[id], sizeof(Record), rec_to_read, manager->inputFPs[id]);
    manager->inputBuffers[id].rec_read += rec_to_read;
    manager->inputBuffers[id].positionInInputBuffer = 0;
    
    return 0;
}


int getNextElement(TapeManager *manager, Record *result, int id) {
    // check if refill is needed
    int position = manager->inputBuffers[id].positionInInputBuffer;
    if (position >= manager->bufferCapacity) {
        if (refillBuffer(manager, id) == 1) {// check if there is no more refill
            return 1;
        }
        *result = manager->inputBuffers[id].buffer[position];
        manager->inputBuffers[id].positionInInputBuffer++;
    }
}

int getTopHeapElement(TapeManager *manager, HeapRecord *result) {
    
    if (manager->heapSize == 0) {
        printf( "Error: Popping top element from an empty heap\n");
        return -1;
    }
    manager->heapSize--;
    HeapRecord topElem = manager->heap[0];
    result->uid1 = topElem.uid1;
    result->uid2 = topElem.uid2;
    result->run_id = topElem.run_id;
    
    manager->heap[0] = manager->heap[1];
    
    return 0;
}

int insertIntoHeap(TapeManager *manager, int id, Record *rec) {
    HeapRecord hrecord;
    hrecord.uid1 = rec->uid1;
    hrecord.uid2 = rec->uid2;
    hrecord.run_id = id;
    
    if (hrecord.uid2 < manager->heap[0].uid2) {
        manager->heap[1] = manager->heap[0];
        manager->heap[0] = hrecord;
    }
    else {
        manager->heap[1] = hrecord;
    }
}

int flushOutputBuffer(TapeManager *manager, int flag) {

    Record * outputBuffer = manager->outputBuffers[flag%2].buffer;
    FILE * outputFP = manager->outputFPs[flag%2];
    int position = manager->outputBuffers[flag%2].positionInInputBuffer;
    fwrite(outputBuffer, sizeof(Record), position, outputFP);
    manager->outputBuffers[flag%2].positionInInputBuffer = 0;  
    
    return 0;
}

int runTapeMergeHelper(TapeManager *manager, int flag) {
    while (manager->heapSize > 0) {
        HeapRecord smallest;
  
        if (getTopHeapElement(manager, &smallest))
            return 1; 

        Record r;
        r.uid1 = smallest.uid1;
        r.uid2 = smallest.uid2;
        int run_id = smallest.run_id;
        int position = manager->outputBuffers[flag%2].positionInOutputBuffer;
        manager->outputBuffers[flag%2].buffer[position] = r;
        manager->outputBuffers[flag%2].positionInOutputBuffer++;
        
        Record next;
        int result = getNextElement(manager, &next, run_id);

        if (result == 0) {//next element exists     
            if (insertIntoHeap (manager, run_id, &next))
                return 1;
        }
        // check if output buffer is full
        if (manager->outputBuffers[flag%2].positionInOutputBuffer == manager->bufferCapacity) {
            flushOutputBuffer(manager, flag);
        }
    }
    
    // flush what remains in output buffer
    if (manager->outputBuffers[flag%2].positionInOutputBuffer) { // there are elements in the output buffer    
        if (flushOutputBuffer(manager, flag))
            return 1;
    }
    
    return 0;
}

int initHeapAndBuffer(TapeManager *manager) {
    // initialise input buffers
    refillBuffer(manager, 0);
    refillBuffer(manager, 1);
    manager->inputBuffers[0].rec_read = 0;
    manager->inputBuffers[1].rec_read = 0;
    manager->outputBuffers[0].positionInOutputBuffer = 0;
    manager->outputBuffers[1].positionInOutputBuffer = 0;
    // initialise heap
    Record r1, r2;
    getNextElement(manager, &r1, 0);
    getNextElement(manager, &r2, 1);
    HeapRecord hr1, hr2;
    hr1.uid1 = r1.uid1;
    hr1.uid2 = r1.uid2;
    hr1.run_id = 0;
    hr2.uid1 = r2.uid1;
    hr2.uid2 = r2.uid2;
    hr2.run_id = 1;
    if (r1.uid2 < r2.uid2) {
        manager->heap[0] = hr1;
        manager->heap[1] = hr2;
    }
    else {
        manager->heap[0] = hr2;
        manager->heap[1] = hr1;
    }
    manager->heapSize = 2;
    return 0;
}

int runTapeMerge(TapeManager *manager) {
    FILE *input1, *input2, *output1, *output2;
    // open all the files
    if (manager->filenameFlag == 0) {
        if (!(input1 = fopen("tape1.dat", "r")) || !(input2 = fopen("tape2.dat", "r")) ||
        !(output1 = fopen("tape3.dat", "w")) || !(output2 = fopen("tape4.dat", "w"))  ) {
            printf("Failed to open tape file(s)\n");
            return -1;
        }
        manager->filenameFlag = 1;
    }
    else {
        if (!(input1 = fopen("tape3.dat", "r")) || !(input2 = fopen("tape3.dat", "r")) ||
        !(output1 = fopen("tape1.dat", "w")) || !(output2 = fopen("tape2.dat", "w"))  ) {
            
            printf("Failed to open tape file(s)\n");
            return -1;
        }
        manager->filenameFlag = 0;
    }
    // initialise the file pointers in manager
    manager->inputFPs[0] = input1;
    manager->inputFPs[1] = input2;
    manager->outputFPs[0] = output1;
    manager->outputFPs[1] = output2;  
     
    long input1_capacity = get_filesize(input1) / sizeof(Record);
    long input2_capacity = get_filesize(input2) / sizeof(Record);
    long partition = manager->rec_per_sublist;
    int iterations = (input1_capacity > input2_capacity) ? 
    (input1_capacity / partition) : (input2_capacity / partition);
    
    for (int i=0; i < iterations; i++) {
        manager->outputBuffers[0].positionInOutputBuffer = 0;
        manager->outputBuffers[1].positionInOutputBuffer = 0;
        initHeapAndBuffer(manager);
        runTapeMergeHelper(manager, i); // depending on i, merged data will either be written in output1 or output 2
        
    }
    fclose(input1);
    fclose(input2);
    fclose(output1);
    fclose(output2);
    
    return 0;
}

int runPhaseTwo(TapeManager *manager) {
    // big loop to do multiple phases
    while(1) { // just running 1 phase for now test other parts: TODO
        runTapeMerge(manager); // do the merging and swap the input-output tapes
        break;
    }
    
    return 0;
}



