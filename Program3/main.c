/*
    Program name: Program 3, Parser Program
    Course: CMSC 4180, Language Translation
    Group: #3
    Members:
        Shawn Gallagher - GAL82896@pennwest.edu
        Joshua Watson - WAT93888@pennwest.edu
        Camron Mellot - MEL98378@pennwest.edu
        Luke Joseph - JOS60794@pennwest.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"
#include "scanner.h"
#include "parser.h"

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
    if (lexical_error_count == 0 && syntax_error_count == 0)
    {
        fprintf(g_listing_file, "Input File Compiled without Errors\n");
    }
    else
    {
        fprintf(g_listing_file, "Input File Compiled with Errors\n");
    }

    fprintf(g_listing_file, "Total Number of Lexical Errors: %d\n", lexical_error_count);
    fprintf(g_listing_file, "Total Number of Syntax Errors: %d\n", syntax_error_count);

    printf("\nParser completed successfully!\n\n");

    // Close everything
    file_close(&g_input_file, &g_input_opened);
    file_close(&g_output_file, &g_output_opened);
    file_close(&g_listing_file, &g_listing_opened);
    file_close(&g_temp1_file, &g_temp1_opened);
    file_close(&g_temp2_file, &g_temp2_opened);

    // Delete temp files
    file_delete(&g_temp1_file, g_temp1_filename, &g_temp1_opened);
    // file_delete(&g_temp2_file, g_temp2_filename, &g_temp2_opened);
}

int main(int argc, char *argv[])
{
    startup(argc, argv);
    system_goal();
    wrapup();
    return 0;
}
