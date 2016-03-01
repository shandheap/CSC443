#include <stdio.h>
#include <stdlib.h>
#include "library.h" 

int main(int argc, char *argv[]) { 
    if (argc != 2) {
        printf("Usage: check_dat <input_file>\n");
        return -1;
    }

    FILE * fp_read;
    if (! (fp_read = fopen(argv[1], "r")) ) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }
    
    int filesize = get_filesize(fp_read);
    int number_of_records = filesize / sizeof(Record);
    Record * buffer = (Record *) malloc(sizeof(Record));

    int max = 0;
    int prev = 0;
    for (int i = 0; i < number_of_records; i++) {
        fread(buffer, sizeof(Record), 1, fp_read);
        if (i == 0) {
            max = buffer->uid2;
            prev = buffer->uid2;
        } else {
            if (buffer->uid2 < max) {
                printf("Error: Records not sorted properly at index %d\n", i);
                return -1;
            }

            max = (buffer->uid2 > max) ? buffer->uid2 : max;
            prev = buffer->uid2;
        }
    }

    printf("All records were sorted correctly\n");
    return 0;
}
