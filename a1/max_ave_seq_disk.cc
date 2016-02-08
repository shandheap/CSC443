#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <map>

typedef struct record {
    int uid1;
    int uid2;
} Record;

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Enter the block size as an argument.\n");
        return -1;
    }

    FILE *fp_read;
    char filename[] = "records.dat";

    /* Open file for reading */
    if ( !(fp_read = fopen(filename, "rb")) ) {
        printf("Could not open file %s for reading.\n", filename);
        return -1;
    }

    long block_size = atol(argv[1]);
    block_size -= block_size % sizeof(Record);
    

    struct timeval start, end;
    double time_elapsed, processing_rate;

    /* Calculate the filesize */
    fseek(fp_read, 0L, SEEK_END);
    long filesize = ftell(fp_read);
    fseek(fp_read, 0L, SEEK_SET);
    int num_records = filesize / sizeof(Record);
    block_size = (block_size > filesize) ? filesize : block_size;
    
    int num_of_blocks = ceil(filesize / double(block_size));
    int records_per_block = block_size / sizeof(Record);

    Record * buffer = (Record *) calloc(records_per_block, sizeof(Record));

    /* Use map to track user follower count */
    map<int, int> users;
    int max_count = 0;

    int rec_rem = (filesize % block_size) / sizeof(Record);
    int rec_count = records_per_block;

    /* Read binary file from disk */
    gettimeofday(&start, NULL);
    for (int i=0; i < num_of_blocks; i++) {
        // Check if last block is not full, if so then read partial block
        if (i == num_of_blocks-1 && rec_rem) {
            rec_count = rec_rem;
            fread(buffer, sizeof(Record), rec_count, fp_read);
        }
        else {
            rec_count = records_per_block;
            fread(buffer, sizeof(Record), rec_count, fp_read);
        }

        for (int j=0; j < rec_count; j++) {
            int uid1 = buffer[j].uid1;
            users[uid1]++;
            if (max_count < users[uid1]) {
                max_count = users[uid1];
            }
        }
    }
    gettimeofday(&end, NULL);

    /* Output max and average follower counts */ 
    double ave_count = (double) num_records / (double) users.size();
    printf("Max follower count is %d.\n", max_count);
    printf("Average follower count is %f.\n", ave_count);
 
    /* Output processing rate */
    time_elapsed = end.tv_sec - start.tv_sec + ((end.tv_usec - start.tv_usec) / 1e6);
    processing_rate = (double) filesize / time_elapsed;
    printf("Data rate: %.3f BPS\n", processing_rate);

    fclose(fp_read);
    free(buffer);
}
