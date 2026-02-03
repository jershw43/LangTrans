#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global variables to store results
char global_input_filename[256];
FILE *global_input_file;
int global_input_opened;

void open_input_file(int argc, char *argv[]) {
    char filename[256];
    FILE *input_file = NULL;
    int opened = 0;
    int got_filename_from_args = 0;
    
    // Check for command-line argument
    if (argc > 1) {
        strcpy(filename, argv[1]);
        got_filename_from_args = 1;
    }
    
    // Main loop - continue until valid file opened
    while (!opened) {
        
        // If we don't have filename from args, prompt user
        if (!got_filename_from_args) {
            printf("Enter input filename: ");
            
            // Read user input
            if (fgets(filename, sizeof(filename), stdin) == NULL) {
                printf("\nNo filename entered. Goodbye!\n");
                exit(0);
            }
            
            // Remove trailing newline if present
            filename[strcspn(filename, "\n")] = '\0';
            
            // Check for empty input (just Enter pressed)
            if (filename[0] == '\0') {
                printf("No filename entered. Goodbye!\n");
                exit(0);
            }
        }
        
        // Check if filename has extension
        if (strchr(filename, '.') == NULL) {
            strcat(filename, ".IN");
        }
        
        // Attempt to open file
        input_file = fopen(filename, "r");
        
        // Check if file opened successfully
        if (input_file == NULL) {
            printf("Error: Cannot open file '%s'\n", filename);
            printf("Please try again.\n\n");
            
            // Reset flag so we prompt again on next iteration
            got_filename_from_args = 0;
            
            // Loop continues (no break/goto/exit used here)
        } else {
            // Success! File opened
            opened = 1;
        }
    }
    
    // Store results in global variables
    strcpy(global_input_filename, filename);
    global_input_file = input_file;
    global_input_opened = 1;
}

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