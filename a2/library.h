typedef struct record {
    int uid1;
    int uid2;
} Record;

#define TEMP_FILE "temp.dat"
#define SORT_FILE "edges_sorted.dat"

int compare(const void *a, const void *b);
long get_filesize(FILE * fp);
