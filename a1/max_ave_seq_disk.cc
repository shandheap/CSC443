#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
    block_size += block_size % sizeof(Record);
    int records_per_block = block_size / sizeof(Record);

    Record * buffer = (Record *) calloc(records_per_block, sizeof(Record));

    clock_t begin, end;
    double time_elapsed, processing_rate;

    /* Calculate the filesize */
    fseek(fp_read, 0L, SEEK_END);
    long filesize = ftell(fp_read);
    fseek(fp_read, 0L, SEEK_SET);
    int num_records = filesize / sizeof(Record);
    block_size = (block_size > filesize) ? filesize : block_size;
    int num_of_blocks = filesize / block_size;

    /* Use map to track user follower count */
    map<int, int> users;
    int max_count = 0;

    /* Read binary file from disk */
    begin = clock();
    for (int i=0; i < num_of_blocks; i++) {
        fread(buffer, sizeof(Record), records_per_block, fp_read);
        for (int j=0; j < records_per_block; j++) {
            int uid1 = buffer[j].uid1;
            users[uid1]++;
            if (max_count < users[uid1]) {
                max_count = users[uid1];
            }
        }
    }
    
    // read the remainder of the file(last remaining incomplete block)
    int rec_rem = num_records - num_of_blocks * records_per_block;
    if (rec_rem > 0) {
        fread(buffer, sizeof(Record), rec_rem, fp_read);
        for (int j=0; j < rec_rem; j++) {
            int uid1 = buffer[j].uid1;
            users[uid1]++;
            // keep track of the maximum
            if (max_count < users[uid1]) {
                max_count = users[uid1];
            }
        }
    }
    end = clock();

    /* Output max and average follower counts */ 
    double ave_count = (double) num_records / (double) users.size();
    printf("Max follower count is %d.\n", max_count);
    printf("Average follower count is %f.\n", ave_count);

    /* Output processing rate */
    time_elapsed = (double) (end - begin) / CLOCKS_PER_SEC;
    processing_rate = (double) filesize / (time_elapsed * 1000000);
    printf("Data rate: %.3f MBPS\n", processing_rate);

    fclose(fp_read);
    free(buffer);
}
