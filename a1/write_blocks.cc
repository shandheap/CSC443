#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <time.h>


#define MAX_CHARS_PER_LINE 32

using namespace std;

typedef struct record {
    int uid1;
    int uid2;
} Record;

/* Parse a given line to extract ids and return a Record instance */
Record * parse_line(char line[]) {
    /* Use strtok with delimiters to get ids */
    char * s1 = strtok(line, ",");
    int shift = strlen(s1) + 1;
    char * s2 = strtok(line + shift, "\r\n");

    /* Allocate new record on heap */
    Record * r = (Record *)  malloc(sizeof(Record));
    r->uid1 = atoi(s1);
    r->uid2 = atoi(s2);

    return r;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Error: Enter the input filename and the block size as arguments\n");
        return -1;
    }
    
    char cur_line[MAX_CHARS_PER_LINE];
    FILE *fp_read;

    /* Open file for reading */
    if ( !(fp_read = fopen(argv[1], "r")) ) {
        printf("Could not open file %s for reading.\n", argv[1]);
        return -1;
    }

    /* Read lines and store ids into records array */
    vector<Record> records;
    while (fgets(cur_line, MAX_CHARS_PER_LINE, fp_read) != NULL) {
        Record * r = parse_line(cur_line);
        records.push_back(*r);
        free(r);
    }

    fclose(fp_read);

    char output_filename[] = "records.dat";

    long block_size = atol(argv[2]);
    long arr_size = records.size() * sizeof(Record);
    block_size += block_size % sizeof(Record);
    block_size = (block_size > arr_size) ? arr_size : block_size;
    long records_per_block = (long) block_size / sizeof(Record);
    long num_of_blocks = (long) arr_size / block_size;
    // calculate the records remaining in the incomplete block
    long rec_rem = records.size() - records_per_block * num_of_blocks;
    
    FILE *fp_write;
    if ( !(fp_write = fopen(output_filename, "wb")) ) {
        printf("Could not open file %s for writing.\n", output_filename);
        return -1;
    }

    /* Write pages to disk as blocks */
    clock_t begin, end;
    double time_elapsed, processing_rate;
    begin = clock();
    for (int i=0; i < num_of_blocks; i++) {
        int shift = i * records_per_block;
        fwrite(&records[shift], sizeof(Record), records_per_block, fp_write);
        fflush(fp_write);
    }
    
    if (rec_rem > 0) {
        fwrite(&records[num_of_blocks * records_per_block], sizeof(Record), rec_rem, fp_write);
        fflush(fp_write);
    }
    
    end = clock();
    /* Output processing rate */
    time_elapsed = (double) (end - begin) / CLOCKS_PER_SEC;
    processing_rate = (double) arr_size / (time_elapsed * 1000000);
    printf("Data rate: %.3f MBPS\n", processing_rate);
    fclose(fp_write);

    /*//Test if array was written to disk properly
    Record * buffer = (Record *) calloc(records_per_block, sizeof(Record));
    if ( !(fp_read = fopen(output_filename, "rb")) ) {
        printf("Could not open file %s for reading.\n", output_filename);
        return -1;
    } */

    /* Read pages from disk as blocks
    for (int i=0; i < num_of_blocks; i++) {
        int shift = i * records_per_block;
        fread(buffer, sizeof(Record), records_per_block, fp_read);

        for (int j=0; j < records_per_block; j++) {
            if (buffer[j].uid1 != records[shift+j].uid1 ||
                buffer[j].uid2 != records[shift+j].uid2) {
                printf("Error: Binary dat records do not match original.\n");
                return -1;
            }
        }
    }

    fclose(fp_read);
    free(buffer);*/

    return 0;
}
