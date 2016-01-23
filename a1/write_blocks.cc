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


int main() {
    char file_name[] = "data/g_plusAnonymized.csv";

    char cur_line[MAX_CHARS_PER_LINE];
    FILE *fp_read;

    /* Open file for reading */
    if ( !(fp_read = fopen(file_name, "r")) ) {
        printf("Could not open file %s for reading.\n", file_name);
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

    /* Allocate buffer for 1 block */
    char output_filename[] = "records.dat";

    /* Write records as blocks to disk */
    int block_size = 1024 * 1024;
    int records_per_block = 1;
    int total_records = records.size();
    FILE *fp_write;

    Record * buffer = (Record *) calloc(records_per_block, sizeof(Record));
    if ( !(fp_write = fopen(output_filename, "wb")) ) {
        printf("Could not open file %s for writing.\n", output_filename);
        return -1;
    }

    fwrite(buffer, sizeof(Record), total_records, fp_write);
    fflush(fp_write);
    fclose(fp_write);
    free(buffer);

    return 0;
}
