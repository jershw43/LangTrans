#include <stdio.h>
#include <stdlib.h>
#include "file_util.c"

int main(int argc, char *argv[]) {
    printf("\n");

    // INPUT
    open_input_file(argc, argv);
    file_status("INPUT", global_input_filename, global_input_file, global_input_opened, PRINT_GREEN);
    // Try to read and display first line of input file
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), global_input_file) != NULL) {
        printf("First line of input file:\n");
        printf("%s%s%s\n", PRINT_ITALIC, buffer, PRINT_RESET);
    }

    // OUTPUT
    open_output_file(argc, argv);
    file_status("OUTPUT", global_output_filename, global_output_file, global_output_opened, PRINT_CYAN);
    
    // LISTING
    open_listing_file();
    file_status("LISTING", global_listing_filename, global_listing_file, global_listing_opened, PRINT_YELLOW);

    // Clean up
    file_close();
    
    return 0;
}
