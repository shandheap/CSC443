#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define MAX_CHARS_PER_LINE 32

using namespace std;

typedef struct record {
    int uid1;
    int uid2;
} Record;

/* Parse a given line to extract ids and return vector of ids (strings) */
vector<string> parse_line(char line[]) {    
    vector<string> parsed; 
    string buf;
 
    /* Parse characters to get ids */
    for (int i=0; i < MAX_CHARS_PER_LINE; i++) {
        if (line[i] == '\r' || line[i] == '\n') {
            string *id2 = new string(buf);
            parsed.push_back(*id2);
            buf.clear();
            break;
        }

        if (line[i] == ',') {
            string *id1 = new string(buf);
            parsed.push_back(*id1);
            buf.clear();
            continue;
        }

        buf += line[i];
    }
    
    return parsed;
}


int main() {
    char file_name[] = "data/g_plusAnonymized.csv";

    char cur_line[MAX_CHARS_PER_LINE];
    FILE *fp_read;

    /* Open file for reading */
    if ( !(fp_read = fopen(file_name, "r")) ) {
        printf("Could not open file %s for reading.\n", file_name);
        return -1;
    }

    /* Read lines and store ids into records array */
    vector<Record> records;
    while (fgets(cur_line, MAX_CHARS_PER_LINE, fp_read) != NULL) {
        vector<string> parsed = parse_line(cur_line);

        Record *cur_record = (Record *) malloc(sizeof(Record));
        cur_record->uid1 = atoi(parsed[0].c_str());
        cur_record->uid2 = atoi(parsed[1].c_str());

        records.push_back(*cur_record);
        parsed.clear();
    }

    fclose(fp_read);

    /* Allocate buffer for 1 block */
    char output_filename[] = "records.dat";

    /* Write records as blocks to disk */
    int block_size = 1024 * 1024;
    int records_per_block = 1;
    int total_records = records.size();
    FILE *fp_write;

    Record * buffer = (Record *) calloc(records_per_block, sizeof(Record));
    if ( !(fp_write = fopen(output_filename, "wb")) ) {
        printf("Could not open file %s for writing.\n", output_filename);
        return -1;
    }

    fwrite(buffer, sizeof(Record), total_records, fp_write);
    fflush(fp_write);
    fclose(fp_write);
    free(buffer);

    return 0;
}
