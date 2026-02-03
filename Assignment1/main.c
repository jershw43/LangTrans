#include <stdio.h>
#include <stdlib.h>
#include "file_util.c"

int main(int argc, char *argv[]) {
    // Call the function
    open_input_file(argc, argv);
    
    // If we get here, file was successfully opened
    printf("\n=== SUCCESS ===\n");
    printf("Filename: %s\n", global_input_filename);
    printf("File pointer: %p\n", (void *)global_input_file);
    printf("Opened status: %d\n", global_input_opened);
    
    // Try to read and display first line of file
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), global_input_file) != NULL) {
        printf("\nFirst line of file:\n%s", buffer);
    }
    
    // Clean up
    fclose(global_input_file);
    
    return 0;
}