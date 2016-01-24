#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct record {
    int uid1;
    int uid2;
} Record;


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Enter the block size as an argument.\n");
        return -1;
    }

    FILE *fp_read;
    char filename[] = "records.dat";

    /* Open file for reading */
    if ( !(fp_read = fopen(filename, "r")) ) {
        printf("Could not open file %s for reading.\n", filename);
        return -1;
    }

    long block_size = atol(argv[1]);
    block_size += block_size % sizeof(Record);
    int records_per_block = block_size / sizeof(Record);

    Record * buffer = (Record *) calloc(records_per_block, sizeof(Record));
    if ( !(fp_read = fopen(filename, "rb")) ) {
        printf("Could not open file %s for reading.\n", filename);
        return -1;
    }

    clock_t begin, end;
    double time_elapsed, processing_rate;

    /* Calculate the filesize */
    fseek(fp_read, 0L, SEEK_END);
    long filesize = ftell(fp_read);
    block_size = (block_size > filesize) ? filesize : block_size;

    /* Read binary file from disk */
    begin = clock();
    for (int i=0; i < filesize / block_size; i++) {
        fread(buffer, sizeof(Record), records_per_block, fp_read);
    }
    end = clock();

    /* Output processing rate */
    time_elapsed = (double) (end - begin) / CLOCKS_PER_SEC;
    processing_rate = (double) filesize / (time_elapsed * 1000000);
    printf("Data rate: %.3f MBPS\n", processing_rate);

    fclose(fp_read);
    free(buffer);
}
