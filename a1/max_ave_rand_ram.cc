#include <sys/mman.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <map>

typedef struct record {
    int uid1;
    int uid2;
} Record;

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Error: Enter the input filename and the block size as arguments\n");
        return -1;
    }

    FILE *fp_read;

    /* Open file for reading */
    if ( !(fp_read = fopen(argv[1], "rb")) ) {
        printf("Could not open file %s for reading.\n", argv[1]);
        return -1;
    }

    long block_size = atol(argv[1]);
    block_size -= block_size % sizeof(Record);
    
    /* Calculate the filesize */
    fseek(fp_read, 0L, SEEK_END);
    long filesize = ftell(fp_read);
    int num_records = filesize / sizeof(Record);
    block_size = (block_size > filesize) ? filesize : block_size;
    int num_of_blocks = ceil(filesize / (double) block_size);

    /* Use map to track user follower count */
    map<int, int> users;
    int max_count = 0;

    /* Create array of random block indexes */
    int rand_blocks[num_of_blocks];
    for (int i=0; i < num_of_blocks; i++)
        rand_blocks[i] = i;
    srand(time(0));
    random_shuffle(&rand_blocks[0], &rand_blocks[num_of_blocks-1]);


    int records_per_block = block_size / sizeof(Record);

    int rec_rem = (filesize % block_size) / sizeof(Record);
    int rec_count = records_per_block;

    struct timeval start, end;
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

    /* Read binary records from RAM */
    gettimeofday(&start, NULL);
    for (int i=0; i < num_of_blocks; i++) {
        // Check if last block is not full, if so then read partial block
        if (rand_blocks[i] == num_of_blocks-1 && rec_rem)
            rec_count = rec_rem;
        else
            rec_count = records_per_block;

        for (int j=0; j < rec_count; j++) {
            // Shift the mmap ptr to a random block
            int cur_block = records_per_block * rand_blocks[i];
            Record * block_iter = (Record *) mmap_ptr + cur_block;
            int uid1 = (block_iter + j) -> uid1;
            users[uid1]++;
            if (max_count < users[uid1]) {
                max_count = users[uid1];
            }
        }
    }
    gettimeofday(&end, NULL);

    /* Clear file from RAM */
    munmap(mmap_ptr, filesize);

    /* Output max and average follower counts */ 
    double ave_count = (double) num_records / (double) users.size();
    printf("Max follower count is %d.\n", max_count);
    printf("Average follower count is %f.\n", ave_count);

    /* Output processing rate */
    time_elapsed = end.tv_sec - start.tv_sec + ((end.tv_usec - start.tv_usec) / 1e6);
    processing_rate = (double) filesize / time_elapsed;
    printf("Data rate: %.3f BPS\n", processing_rate);
}
