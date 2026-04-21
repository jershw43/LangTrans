/*
    Program name: Program 4, Code Generation Program
    Course: CMSC 4180, Language Translation
    Group: #3
    Members:
        Shawn Gallagher - GAL82896@pennwest.edu
        Joshua Watson - WAT93888@pennwest.edu
        Camron Mellot - MEL98378@pennwest.edu
        Luke Joseph - JOS60794@pennwest.edu
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_util.h"
#include "scanner.h"
#include "parser.h"
#include "action.h"

void startup(int argc, char *argv[])
{
    printf("\n");

    // Input file
    init_input_file(argc, argv);
    file_open(&g_input_file, g_input_filename, "r", &g_input_opened);
    file_status("INPUT", g_input_filename, g_input_file, &g_input_opened);
    printf("\n");

    do
    {
        // Output file
        init_output_file(argc, argv);
        file_open(&g_output_file, g_output_filename, "w", &g_output_opened);
        file_status("OUTPUT", g_output_filename, g_output_file, &g_output_opened);
        printf("\n");

        // Listing file
        init_listing_file();
        file_open(&g_listing_file, g_listing_filename, "w", &g_listing_opened);
        file_status("LISTING", g_listing_filename, g_listing_file, &g_listing_opened);
    }
    while (!validate_names());

    // Temp files
    init_temp_files();
}

void wrapup(void)
{
    // Print summary to listing file before closing
    fprintf(g_listing_file, "Total number of Lexical Errors: %d\n", lexical_error_count);
    fprintf(g_listing_file, "Total number of Syntax Errors: %d\n", syntax_error_count);
    if (lexical_error_count == 0 && syntax_error_count == 0)
        fprintf(g_listing_file, "Program compiled without errors.\n");
    else
        fprintf(g_listing_file, "Program compiled with errors.\n");

    // Close everything
    file_close(&g_input_file, &g_input_opened);
    file_close(&g_output_file, &g_output_opened);
    file_close(&g_listing_file, &g_listing_opened);
    file_close(&g_temp1_file, &g_temp1_opened);
    file_close(&g_temp2_file, &g_temp2_opened);

    // Delete temp files
    // file_delete(&g_temp1_file, g_temp1_filename, &g_temp1_opened);
    // file_delete(&g_temp2_file, g_temp2_filename, &g_temp2_opened);
}

int run_generated_code(void) {
    char choice[10];
    char exe_name[256];
    char compile_cmd[512];
    char run_cmd[512];
    int status = 0;

    printf("\nCode generation completed successfully!\n");
    printf("Would you like to run %s? (Y/N): ", g_output_filename);
    scanf(" %c", choice);

    if (choice[0] == 'Y' || choice[0] == 'y') {
        // Copy filename and strip ".c"
        strncpy(exe_name, g_output_filename, sizeof(exe_name));
        exe_name[sizeof(exe_name) - 1] = '\0';

        char *dot = strrchr(exe_name, '.');
        *dot = '\0';

        // Define compile + run commands (platform dependent)
        #ifdef _WIN32
            snprintf(compile_cmd, sizeof(compile_cmd), "gcc \"%s\" -o \"%s.exe\"", g_output_filename, exe_name);
            snprintf(run_cmd, sizeof(run_cmd), "\"%s.exe\"", exe_name);
        #else
            snprintf(compile_cmd, sizeof(compile_cmd), "gcc \"%s\" -o \"%s\"", g_output_filename, exe_name);
            snprintf(run_cmd, sizeof(run_cmd), "./\"%s\"", exe_name);
        #endif

        // Actually compile + run
        printf("Compiling %s...\n", g_output_filename);
        if (system(compile_cmd) != 0)
        {
            printf("Compilation failed.\n");
            status = 1;
        }
        else
        {
            printf("Running %s...\n", exe_name);
            if (system(run_cmd) != 0)
            {
                printf("Execution failed.\n");
                status = 2;
            }
        }
    }
    else
    {
        printf("Exiting program.\n");
    }

    printf("\n");
    return status;
}

int main(int argc, char *argv[])
{
    startup(argc, argv);
    system_goal();
    wrapup();
    run_generated_code();
    return 0;
}
