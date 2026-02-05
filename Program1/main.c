#include <stdio.h>
#include <stdlib.h>
#include "file_util.c"

#define PRINT_RED     "\x1b[31m"
#define PRINT_YELLOW  "\x1b[33m"
#define PRINT_GREEN   "\x1b[32m"
#define PRINT_BLUE    "\x1b[34m"
#define PRINT_CYAN    "\x1b[36m"
#define PRINT_ITALIC  "\x1b[3m"
#define PRINT_RESET   "\x1b[0m"

int main(int argc, char *argv[]) {
    printf("\n");

    // INPUT
    open_input_file(argc, argv);
    printf("\n%s[INPUT FILE STATUS]%s\n", PRINT_GREEN, PRINT_RESET);
    printf("– Filename: %s\n", global_input_filename);
    printf("– File pointer: %p\n", (void *)global_input_file);
    printf("– Currently opened: %s\n\n", global_input_opened ? "true" : "false");
    // Try to read and display first line of input file
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), global_input_file) != NULL) {
        printf("First line of input file:\n");
        printf("%s%s%s\n", PRINT_ITALIC, buffer, PRINT_RESET);
    }

    // OUTPUT
    open_output_file(argc, argv);
    printf("\n%s[OUTPUT FILE STATUS]%s\n", PRINT_CYAN, PRINT_RESET);
    printf("– Filename: %s\n", global_output_filename);
    printf("– File pointer: %p\n", (void *)global_output_file);
    printf("– Currently opened: %s\n\n", global_output_opened ? "true" : "false");
    
    // LISTING
    open_listing_file();
    printf("%s[LISTING FILE STATUS]%s\n", PRINT_YELLOW, PRINT_RESET);
    printf("– Filename: %s\n", global_listing_filename);
    printf("– File pointer: %p\n", (void *)global_listing_file);
    printf("– Currently opened: %s\n\n", global_listing_opened ? "true" : "false");

    // Clean up
    file_close();
    
    return 0;
}
