#include <stdio.h>
#include "library.h"


/**
 * * Compares two records a and b 
 * * with respect to the value of the integer field f.
 * * Returns an integer which indicates relative order: 
 * * positive: record a > record b
 * * negative: record a < record b
 * * zero: equal records
 * */
int compare(const void *a, const void *b) {
    int a_uid2 = ((const Record *) a)->uid2; 
    int b_uid2 = ((const Record *) b)->uid2;

    return a_uid2 - b_uid2;
}

/*
 * Get the filesize of the file with pointer fp.
 * Returns an integer which indicates the filesize.
 */
long get_filesize(FILE * fp) {
    long filesize;

    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    return filesize;
}
