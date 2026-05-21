#include <stdio.h>
#include <stdlib.h>

#include "file_io.h"


char* slurpFile(const char* filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open file input for reading! (Does it exist?)\n");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long  size = ftell(f);
    rewind(f);

    char *s = malloc((size + 1) * sizeof(*s));
    fread(s, 1, size, f);
    s[size] = '\0';

    fclose(f);

    return s;
}


void spitFile(const char* filename, char* s) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open file output.txt for writing! (Do I have perms?)\n");
        exit(2);
    }

    fputs(s, f);
    fclose(f);
}
