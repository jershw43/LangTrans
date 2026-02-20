#include "file_util.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Global definitions
FILE *g_input_file  = NULL;
FILE *g_output_file = NULL;
FILE *g_listing_file = NULL;
FILE *g_backup_file = NULL;
FILE *g_temp1_file = NULL;
FILE *g_temp2_file = NULL;

char g_input_filename[MAX_FILENAME] = "";
char g_output_filename[MAX_FILENAME] = "";
char g_listing_filename[MAX_FILENAME] = "";
char g_backup_filename[MAX_FILENAME] = "";
char g_temp1_filename[MAX_FILENAME] = "temp1.tmp";
char g_temp2_filename[MAX_FILENAME] = "temp2.tmp";

int g_input_opened = 0;
int g_output_opened = 0;
int g_backup_opened = 0;
int g_listing_opened = 0;
int g_temp1_opened = 0;
int g_temp2_opened = 0;

void init_input_file(int argc, char *argv[])
{
    char filename[256];
    FILE *input_file = NULL;
    int opened = 0;
    int got_filename_from_args = 0;
    
    // Check for command-line argument
    if (argc > 1)
    {
        strcpy(filename, argv[1]);
        got_filename_from_args = 1;
    }
    
    // Main loop - continue until valid file opened
    while (!opened)
    {
        // If we don't have filename from args, prompt user
        if (!got_filename_from_args)
        {
            printf("Enter input filename: ");
            
            // Read user input
            if (fgets(filename, sizeof(filename), stdin) == NULL)
            {
                printf("\nNo filename entered.\n");
                exit(0);
            }
            
            // Remove trailing newline if present
            filename[strcspn(filename, "\n")] = '\0';
        }
        
        // Check if filename has extension
        if (strchr(filename, '.') == NULL)
        {
            strcat(filename, IN);
        }
        
        // Attempt to open file
        input_file = fopen(filename, "r");
        if (input_file == NULL)
        {
            printf("Error: Cannot open file '%s'\n", filename);
            printf("Please try again.\n\n");
            got_filename_from_args = 0;
        }
        else
        {
            opened = 1;
        }
    }
    
    // Store results in global variables
    strcpy(g_input_filename, filename);
    g_input_file = input_file;
    g_input_opened = 1;
}

void init_output_file(int argc, char *argv[])
{
    char filename[256];
    FILE *output_file = NULL;
    int opened = 0;
    int got_filename_from_args = 0;
    //Flag to prevent unwanted overwrite
    int overwrite_allowed = 0;
    
    // Check for command-line argument
    if (argc > 2)
    {
        strcpy(filename, argv[2]);
        got_filename_from_args = 1;
    }
    
    // Main loop - continue until valid file opened
    while (!opened)
    {
        // If we don't have filename from args, prompt user
        if (!got_filename_from_args)
        {
            printf("Enter output filename: ");
            
            // Read user input
            if (fgets(filename, sizeof(filename), stdin) == NULL)
            {
                printf("\nNo filename entered. Using default from input file.\n");
                // Generate from input filename
                strcpy(filename, g_input_filename);
                char *dot = strrchr(filename, '.');
                if (dot != NULL)
                {
                    strcpy(dot, OUT);
                }
                else
                {
                    strcat(filename, OUT);
                }
            }
            else
            {
                // Remove trailing newline
                filename[strcspn(filename, "\n")] = '\0';
                
                // Check for empty input
                if (filename[0] == '\0')
                {
                    printf("No filename entered. Using default from input file.\n");
                    // Generate from input filename
                    strcpy(filename, g_input_filename);
                    char *dot = strrchr(filename, '.');
                    if (dot != NULL)
                    {
                        strcpy(dot, OUT);
                    }
                    else
                    {
                        strcat(filename, OUT);
                    }
                }
            }
        }
        
        // Check if filename has extension
        if (strchr(filename, '.') == NULL)
        {
            strcat(filename, OUT);
        }
        
        // Check if file exists
        if (access(filename, F_OK) == 0)
        {
            if(!overwrite_allowed)
            {
                // File exists, prompt for choice
                int choice_made = 0;
                while (!choice_made)
                {
                    printf("File '%s' already exists.\n", filename);
                    printf("Choose an option:\n");
                    printf("1. Overwrite the file (backup will be created)\n");
                    printf("2. Enter a new filename\n");
                    printf("3. Quit the program\n");
                    printf("Enter your choice: ");
                    
                    char choice[10];
                    if (fgets(choice, sizeof(choice), stdin) == NULL)
                    {
                        printf("Invalid input. Goodbye!\n");
                        exit(0);
                    }
                    choice[strcspn(choice, "\n")] = '\0';
                    
                    if (strcmp(choice, "1") == 0)
                    {  
                        //Set flag, allowing for overwrite
                        overwrite_allowed = 1;
                        // Backup the existing file
                        char temp[MAX_FILENAME];
                        strcpy(temp, g_output_filename);
                        strcpy(g_output_filename, filename);
                        backup_output();
                        strcpy(g_output_filename, temp);
                        
                        // Now open for writing
                        output_file = fopen(filename, "w");
                        if (output_file == NULL)
                        {
                            printf("Error: Cannot open file '%s' for writing\n", filename);
                            printf("Please try again.\n");
                            got_filename_from_args = 0;
                            choice_made = 1;  // Exit choice loop, retry outer loop
                        }
                        else
                        {
                            opened = 1;
                            choice_made = 1;
                        }
                    }
                    else if (strcmp(choice, "2") == 0)
                    {
                        // Enter new name
                        got_filename_from_args = 0;
                        choice_made = 1;  // Exit choice loop, retry outer loop
                    }
                    else if (strcmp(choice, "3") == 0)
                    {
                        printf("Terminating program.\n");
                        exit(0);
                    }
                    else
                    {
                        printf("Invalid choice. Please try again.\n");
                        // choice_made remains 0, loop again for choice
                    }
                }
            }
        }
        else
        {
            // File doesn't exist, open for writing
            output_file = fopen(filename, "w");
            if (output_file == NULL)
            {
                printf("Error: Cannot open file '%s' for writing\n", filename);
                printf("Please try again.\n\n");
                got_filename_from_args = 0;
            }
            else
            {
                opened = 1;
            }
        }
    }

    // Store results in global variables
    strcpy(g_output_filename, filename);
    g_output_file = output_file;
    g_output_opened = 1;

    printf("\n");
}

void init_listing_file()
{
    // Derive listing filename from output filename
    strcpy(g_listing_filename, g_output_filename);
    char *dot = strrchr(g_listing_filename, '.');
    if (dot != NULL)
    {
        strcpy(dot, LIS);
    }
    else
    {
        strcat(g_listing_filename, LIS);
    }
}

void init_temp_files()
{
    // Temp 1
    file_open(&g_temp1_file, g_temp1_filename, "w", &g_temp1_opened);
    fprintf(g_temp1_file, "The Temp\n");
    char ch;
    while ((ch = (char)fgetc(g_temp1_file)) != EOF)
    {
        fputc(ch, g_output_file);
    }
    file_close(&g_temp1_file, &g_temp1_opened);

    // Temp 2
    file_open(&g_temp2_file, g_temp2_filename, "w", &g_temp2_opened);
    fprintf(g_temp2_file, "Loop Information\n");
    file_close(&g_temp2_file, &g_temp2_opened);
}

int validate_names(void)
{
    if (strcmp(g_input_filename, g_output_filename) == 0)
    {
        printf("Error: Input file name must be different from output file name.\n");
        return 0;
    }
    else if (strcmp(g_input_filename, g_listing_filename) == 0)
    {
        printf("Error: Input file name must be different from listing file name.\n");
        return 0;
    }
    else if (strcmp(g_input_filename, g_backup_filename) == 0)
    {
        printf("Error: Input file name must be different from backup file name.\n");
        return 0;
    }
    return 1;
}

void generate_extensions()
{
    char *dot;

    // Input
    dot = strrchr(g_input_filename, '.');
    if (dot == NULL)
    {
        strcat(g_input_filename, IN);
    }

    // Output
    if (g_output_filename[0] == '\0')
    {
        strcpy(g_output_filename, g_input_filename);
        dot = strrchr(g_output_filename, '.');
        if (dot != NULL)
        {
            strcpy(dot, OUT);
        }
    }
    else
    {
        dot = strrchr(g_output_filename, '.');
        if (dot == NULL)
        {
            strcat(g_output_filename, OUT);
        }
    }

    // Listing
    strcpy(g_listing_filename, g_output_filename);
    dot = strrchr(g_listing_filename, '.');
    if (dot != NULL)
    {
        strcpy(dot, LIS);
    }
}

void backup_output()
{
    char backup_name[MAX_FILENAME];
    char *dot;

    // Build backup filename
    strcpy(backup_name, g_output_filename);
    dot = strrchr(backup_name, '.');
    if (dot != NULL)
    {
        strcpy(dot, BAK);
    }
    else
    {
        strcat(backup_name, BAK);
    }
    
    // Remove old backup if it exists
    remove(backup_name);

    // Rename current output to backup
    rename(g_output_filename, backup_name);
}

int file_open(FILE **file, const char *filename, const char *mode, int *is_opened)
{
    if (file == NULL || filename == NULL || mode == NULL || is_opened == NULL) return 0;

    *file = fopen(filename, mode);
    if (*file != NULL)
    {
        *is_opened = 1;
        return 1;
    }
    *is_opened = 0;

    return 0;
}

void file_close(FILE **file, int *is_opened)
{
    if (file != NULL && *file != NULL)
    {
        fclose(*file);
        *file = NULL;
        *is_opened = 0;
    }
}

void file_delete(FILE **file, const char *filename, int *is_opened)
{
    remove(filename);
    if (file != NULL) *file = NULL;
    *is_opened = 0;
}

void file_status(const char *name, const char *filename, FILE *file, int *is_opened)
{
    printf("[%s FILE STATUS]\n", name ? name : "(null)");
    printf("– Filename: %s\n", filename ? filename : "(null)");
    printf("– File pointer: %p\n", (void *)file);
    printf("– Currently opened: %s\n", (is_opened && *is_opened) ? "true" : "false");
}
