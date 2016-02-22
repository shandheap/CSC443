typedef struct record {
    int uid1;
    int uid2;
} Record;

#define TEMP_FILE "temp.dat"

int compare(const void *a, const void *b);
long get_filesize(FILE * fp);
