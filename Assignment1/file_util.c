#include "file_util.h"
#include <string.h>
#include <stdlib.h>

/* ---------- GLOBAL DEFINITIONS ---------- */
char global_input_filename[MAX_FILENAME] = "";
char global_output_filename[MAX_FILENAME] = "";
char global_listing_filename[MAX_FILENAME] = "";

FILE *global_input_file  = NULL;
FILE *global_output_file = NULL;
FILE *global_listing_file = NULL;

extern int global_input_opened;
int global_input_opened = 0;

void open_input_file(int argc, char *argv[])
{
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
void open_output_file()
{

}
void open_listing_file()
{

}
//Josh
void create_temp_files() {
    FILE *fp;
    
    // Create temp1.
    fp = fopen("temp1.IN", "w");
    if (fp != NULL) {
        fprintf(fp, "Temporary File 1\n");
        fclose(fp);
        printf("Created: temp1.IN\n");
    }
    // Create temp2
    fp = fopen("temp2.IN", "w");
    if (fp != NULL)
    {
        fprintf(fp, "Temporary File 2 \n");
        fclose(fp);
        printf("Created: temp2.IN\n");
    }
    
}
//Luke
void validate_names()
{

}

//Camron
void generate_extensions()
{
    char *dot;

    /* ---------- INPUT FILE ---------- */
    /* Add .IN if no extension exists */
    dot = strrchr(global_input_filename, '.');
    if (dot == NULL) {
        strcat(global_input_filename, IN);
    }

    /* ---------- OUTPUT FILE ---------- */
    /* If no output filename provided, derive from input */
    if (global_output_filename[0] == '\0') {
        strcpy(global_output_filename, global_input_filename);

        /* Replace extension with .OUT */
        dot = strrchr(global_output_filename, '.');
        if (dot != NULL) {
            strcpy(dot, OUT);
        }
    } else {
        /* Output name exists but may lack extension */
        dot = strrchr(global_output_filename, '.');
        if (dot == NULL) {
            strcat(global_output_filename, OUT);
        }
    }

    /* ---------- LISTING FILE ---------- */
    /* Listing file always derived from output file */
    strcpy(global_listing_filename, global_output_filename);
    dot = strrchr(global_listing_filename, '.');
    if (dot != NULL) {
        strcpy(dot, LIS);
    }
}

//Camron
void backup_output()
{
    FILE *fp;
    char backup_name[MAX_FILENAME];
    char *dot;

    /* Check if output file exists */
    fp = fopen(global_output_filename, "r");
    if (fp != NULL) {
        fclose(fp);

        /* Build backup filename */
        strcpy(backup_name, global_output_filename);
        dot = strrchr(backup_name, '.');

        if (dot != NULL) {
            strcpy(dot, BAK);
        } else {
            strcat(backup_name, BAK);
        }

        /* Remove old backup if it exists */
        remove(backup_name);

        /* Rename current output to backup */
        rename(global_output_filename, backup_name);
    }
}

// File Close
void file_close(void)
{
    if (global_input_file != NULL) {
        fclose(global_input_file);
        global_input_file = NULL;
    }

    if (global_output_file != NULL) {
        fclose(global_output_file);
        global_output_file = NULL;
    }

    if (global_listing_file != NULL) {
        fclose(global_listing_file);
        global_listing_file = NULL;
    }

    /* temp file deletion will be added later */
}