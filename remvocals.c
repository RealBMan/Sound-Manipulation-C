#include <stdio.h>
#include <stdlib.h>
 
#define HEADER_SIZE 44
 
int main(int argc, char**argv){
    if (argc != 3) {
        fprintf(stderr, "No sufficient files added\n");
        return 1;
    }
 
    short data[HEADER_SIZE];
    int error;
 
    short value[2];
    short *combined;
 
    FILE *original_file = fopen(argv[1], "rb");
 
    FILE *second_file = fopen(argv[2], "wb");
 
    if (original_file == NULL) {
        fprintf(stderr, "Cannot open file\n");
        return 1;
    }
 
    if (second_file == NULL) {
        fprintf(stderr, "Cannot open file\n");
        return 1;
    }
 
    fread(data, HEADER_SIZE, 1, original_file);
    error = fwrite(data, HEADER_SIZE, 1, second_file);
 
    if (error != 0) {
        fprintf(stderr, "Could not write to the file\n");
        return 1;
    }
 
    while (fread(value, sizeof(short), 2, original_file) == 1){
        *combined = (value[1] - value[0]) / 2;
        error = fwrite(combined, sizeof(short), 1, second_file);
        if (error != 0) {
            fprintf(stderr, "Could not write to the file\n");
            return 1;
        }
        error = fwrite(combined, sizeof(short), 1, second_file);
        if (error != 0) {
            fprintf(stderr, "Could not write to the file\n");
            return 1;
        }
    }
 
    fclose(original_file);
    fclose(second_file);
    return 0;
}
