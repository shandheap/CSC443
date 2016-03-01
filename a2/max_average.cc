#include <stdio.h>
#include <stdlib.h>
#include "library.h" 

int main(int argc, char *argv[]) { 
    if (argc != 3) {
        printf("Usage: max_average <input_file> <block_size>\n");
        return -1;
    }

    FILE * fp_read;
    if (! (fp_read = fopen(argv[1], "r")) ) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }

    int block_size = atoi(argv[2]);
    if (block_size % sizeof(Record)) {
        block_size -= (block_size % sizeof(Record));
    }

    int filesize = get_filesize(fp_read);
    int number_of_records = filesize / sizeof(Record);
    int num_blocks = filesize / block_size;
    int num_records = block_size / sizeof(Record);

    Record * buffer = (Record *) calloc(num_records, sizeof(Record));
    
    int max_followers = 0;
    int avg_followers = 0;
    int unique_followers = 0;
    int prev_uid2 = 0;
    int cur_followers = 0;

    for (int i = 0; i < num_blocks; i++) {
        fread(buffer, sizeof(Record), num_records, fp_read);
        for (int j = 0; j < num_records; j++) {
            int cur_uid2 = buffer[j].uid2;

            if (cur_uid2 > max_followers)
                max_followers = cur_uid2;
            
            if (prev_uid2 != cur_uid2) {
                prev_uid2 = cur_uid2;
                unique_followers++;
                avg_followers += cur_followers;
            } else {
                cur_followers++;
            }
        }
    }

    avg_followers /= (double) unique_followers;

    printf("Maximum number of followers is %d\n", max_followers);
    printf("Average number of followers is %d\n", avg_followers);
    
    return 0;
}
