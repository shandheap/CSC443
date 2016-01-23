#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

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

    int block_size = atoi(argv[2]);
    int record_size = (int) sizeof(Record);
    int arr_size = records.size() * record_size;
    block_size += block_size % record_size;
    int records_per_block = block_size / record_size;
    int num_of_blocks = arr_size / block_size;
    
    FILE *fp_write;
    if ( !(fp_write = fopen(output_filename, "wb")) ) {
        printf("Could not open file %s for writing.\n", output_filename);
        return -1;
    }

    /* Write pages to disk as blocks */
    for (int i=0; i < num_of_blocks; i++) {
        int shift = i * records_per_block;
        fwrite(&records[shift], sizeof(Record), records_per_block, fp_write);
        fflush(fp_write);
    } 
    
    fclose(fp_write);
    
    return 0;
}
