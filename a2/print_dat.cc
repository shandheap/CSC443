#include <stdio.h>
#include <stdlib.h>
#include "library.h" 

int main(int argc, char *argv[]) { 
    
    if (argc != 2) {
        printf("Usage: print_dat <input_file>\n");
        return -1;
    }

    FILE * fp_read;
    if (! (fp_read = fopen(argv[1], "r")) ) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }
    
    int filesize = get_filesize(fp_read);
    int number_of_records = filesize / sizeof(Record);
    Record * buffer = (Record *) calloc(number_of_records, sizeof(Record));
    fread(buffer, sizeof(Record), number_of_records, fp_read);
    
    for (int i = 0; i < number_of_records; i++) {
        printf("%d, %d\n", buffer[i].uid1, buffer[i].uid2);
    }
    
}
