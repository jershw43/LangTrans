#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_util.h"

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
    int got_filename_from_args = (argc > 1);
    
    // Check for command-line argument
    if (got_filename_from_args)
    {
        strcpy(filename, argv[1]);
        filename[sizeof(filename) - 1] = '\0';
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
    int got_filename_from_args = (argc > 2);
    int overwrite_allowed = 0;

    if (got_filename_from_args)
    {
        strcpy(filename, argv[2]);
        filename[sizeof(filename) - 1] = '\0';
    }

    while (!opened)
    {
        int retry = 0;

        if (!got_filename_from_args)
        {
            printf("Enter output filename: ");

            if (fgets(filename, sizeof(filename), stdin) == NULL ||
                filename[0] == '\n')
            {
                printf("No filename entered. Using default from input file.\n");
                strncpy(filename, g_input_filename, sizeof(filename));
                filename[sizeof(filename) - 1] = '\0';
            }
            else
            {
                filename[strcspn(filename, "\n")] = '\0';
            }
        }

        // Normalize extension to .c
        char *dot = strrchr(filename, '.');
        if (dot == NULL)
        {
            strcat(filename, OUT);
        }
        else if (strcmp(dot, OUT) != 0)
        {
            printf("Warning: replacing extension '%s' with '%s'.\n", dot, OUT);
            *dot = '\0';
            strcat(filename, OUT);
        }

        if (access(filename, F_OK) == 0 && !overwrite_allowed)
        {
            char choice[10];
            printf("File '%s' already exists.\n", filename);
            printf("Choose an option:\n");
            printf("1. Overwrite the file (backup will be created)\n");
            printf("2. Enter a new filename\n");
            printf("3. Quit the program\n");
            printf("Enter your choice: ");

            if (fgets(choice, sizeof(choice), stdin) == NULL)
                exit(0);

            if (choice[0] == '1')
            {
                overwrite_allowed = 1;
                char temp[256];
                strcpy(temp, g_output_filename);
                strcpy(g_output_filename, filename);
                backup_output();
                strcpy(g_output_filename, temp);
            }
            else if (choice[0] == '2')
            {
                got_filename_from_args = 0;
                retry = 1;
            }
            else
            {
                printf("Terminating program.\n");
                exit(0);
            }
        }

        if (!retry)
        {
            output_file = fopen(filename, "w");
            if (output_file == NULL)
            {
                printf("Error: Cannot open '%s'. Try again.\n\n", filename);
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
    file_open(&g_temp1_file, g_temp1_filename, "w", &g_temp1_opened);
    file_open(&g_temp2_file, g_temp2_filename, "w", &g_temp2_opened);
}

int validate_names(void)
{
    int result = 1;

    if (strcmp(g_input_filename, g_output_filename) == 0)
    {
        printf("Error: Input file name must be different from output file name.\n");
        result = 0;
    }
    else if (strcmp(g_input_filename, g_listing_filename) == 0)
    {
        printf("Error: Input file name must be different from listing file name.\n");
        result = 0;
    }
    else if (strcmp(g_input_filename, g_backup_filename) == 0)
    {
        printf("Error: Input file name must be different from backup file name.\n");
        result = 0;
    }

    return result;
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
    int result = 0;

    if (file != NULL && filename != NULL && mode != NULL && is_opened != NULL)
    {
        *file = fopen(filename, mode);
        if (*file != NULL)
        {
            *is_opened = 1;
            result = 1;
        }
        else
        {
            *is_opened = 0;
        }
    }

    return result;
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
    printf("- Filename: %s\n", filename ? filename : "(null)");
    printf("- File pointer: %p\n", (void *) file);
    printf("- Currently opened: %s\n", (is_opened && *is_opened) ? "true" : "false");
}