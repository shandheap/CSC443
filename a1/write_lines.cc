#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define MAX_CHARS_PER_LINE 32

typedef struct record {
    int uid1;
    int uid2;
} Record;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Enter the input as an argument\n");
        return -1;
    }
    
    char cur_line[MAX_CHARS_PER_LINE];
    FILE *fp_read;
    char output_filename[] = "records.txt";
    FILE *fp_write;

    /* Open file for reading */
    if ( !(fp_read = fopen(argv[1], "r")) ) {
        printf("Could not open file %s for reading.\n", argv[1]);
        return -1;
    }
    
    /* Open file for writing */
    if ( !(fp_write = fopen(output_filename, "wb")) ) {
        printf("Could not open file %s for writing.\n", output_filename);
        return -1;
    }

    struct timeval start, end;
    double time_spent = 0;
    long bytes_written = 0;
    /* Read lines and write them immediate into another text file */
    while (fgets(cur_line, MAX_CHARS_PER_LINE, fp_read) != NULL) {
        bytes_written += (double) strlen(cur_line) * sizeof(char);

        gettimeofday(&start, NULL);
        fwrite(&cur_line, sizeof(char), strlen(cur_line), fp_write);
        fflush(fp_write);
        gettimeofday(&end, NULL);
        
        time_spent += end.tv_sec - start.tv_sec + ((end.tv_usec - start.tv_usec) / 1e6);
    }

    fclose(fp_read);
    fclose(fp_write);

    printf ("Data rate: %.3f MBps\n", ((bytes_written*sizeof(char))/time_spent)/1000000);

    return 0;
}
