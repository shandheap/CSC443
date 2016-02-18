#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "library.h"


int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <input_file> <total_mem> <block_size> <num_of_runs>\n", argv[0]);
        return -1;
    }

    FILE * fp_read;
    if (! (fp_read = fopen(argv[1], "r")) ) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }

    int total_mem = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    int num_of_runs = atoi(argv[4]);

    int filesize = get_filesize(fp_read);

    int chunk_size = filesize / num_of_runs;
    int total_running_mem = 0;
    int records_per_chunk = chunk_size / sizeof(Record);

    FILE * fp_write;
    if (! (fp_write = fopen("sorted_phase1.dat", "w")) ) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }

    Record * buffer = (Record *) calloc(records_per_chunk, sizeof(Record));
    for (int i=0; i < num_of_runs; i++) {
        fread(buffer, sizeof(Record), records_per_chunk, fp_read);
        qsort(buffer, records_per_chunk, sizeof(Record), compare);
        fwrite(buffer, sizeof(Record), records_per_chunk, fp_write);
    }

    fclose(fp_read);
    fclose(fp_write);

    return 0;
}
