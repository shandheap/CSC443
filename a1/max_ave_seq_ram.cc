#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
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
    
    /* Calculate the filesize */
    fseek(fp_read, 0L, SEEK_END);
    long filesize = ftell(fp_read);
    fseek(fp_read, 0L, SEEK_SET);
    int num_records = filesize / sizeof(Record);
    block_size = (block_size > filesize) ? filesize : block_size;
    int num_of_blocks = ceil(filesize / (double) block_size);

    int records_per_block = block_size / sizeof(Record);
    int rec_rem = (filesize % block_size) / sizeof(Record);
    int rec_count = records_per_block;

    clock_t begin, end;
    double time_elapsed, processing_rate;

    /* Load the file into RAM */
    int fd = fileno(fp_read);
    void * mmap_ptr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!mmap_ptr) {
        printf("Could not load the file into RAM.\n");
        return -1;
    }

    /* Close file stream */
    fclose(fp_read);

    /* Warm up RAM cache with trial mmap traversal */
    for (int i=0; i < num_records; i++) {
        int uid1 = ((Record *) mmap_ptr + i) -> uid1;
        int uid2 = ((Record *) mmap_ptr + i) -> uid2;
    }

    /* Use map to track user follower count */
    map<int, int> users;
    int max_count = 0;

    /* Read binary records from RAM */
    begin = clock();
    for (int i=0; i < num_of_blocks; i++) {
        // Check if last block is not full, if so then read partial block
        if (i == num_of_blocks-1 && rec_rem)
            rec_count = rec_rem;
        else
            rec_count = records_per_block;

        for (int j=0; j < rec_count; j++) {
            // Shift the mmap ptr to a block
            int cur_block = records_per_block * i;
            Record * block_iter = (Record *) mmap_ptr + cur_block;
            int uid1 = (block_iter + j) -> uid1;
            users[uid1]++;
            if (max_count < users[uid1]) {
                max_count = users[uid1];
            }
        }
    }
    end = clock();

    /* Clear file from RAM */
    munmap(mmap_ptr, filesize);

    /* Output max and average follower counts */ 
    double ave_count = (double) num_records / (double) users.size();
    printf("Max follower count is %d.\n", max_count);
    printf("Average follower count is %f.\n", ave_count);

    /* Output processing rate */
    time_elapsed = (double) (end - begin) / CLOCKS_PER_SEC;
    processing_rate = (double) filesize / (time_elapsed * 1000000);
    printf("Data rate: %.3f MBPS\n", processing_rate);
}
