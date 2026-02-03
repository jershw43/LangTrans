#include "file_util.h"
#include <stdio.h>

FILE *fp = NULL;

void init_file_open() {
    // Initialization of the file open process
    // For example, prepare file path or check permissions
    printf("Initializing file open process...\n");
}

void open_file() {
    // Carry out the file open
    fp = fopen("example.txt", "r");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    printf("File opened successfully.\n");
}

void wrap_up() {
    // Wrap up: close the file and clean up
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
        printf("File closed and process wrapped up.\n");
    }
}
