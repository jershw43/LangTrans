/*
	Program name: Assignment 1, File Open Program
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

int main(int argc, char *argv[])
{
    printf("\n");

    // Input
    init_input_file(argc, argv);
    file_open(global_input_file, "r", global_input_opened);
    file_status("INPUT", global_input_filename, global_input_file, global_input_opened);

    do
    {
        // Output
        init_output_file(argc, argv);
        file_open(global_output_file, "w", global_output_opened);
        file_status("OUTPUT", global_output_filename, global_output_file, global_output_opened);

        // Listing
        init_listing_file();
        file_open(global_listing_file, "w", global_listing_opened);
        file_status("LISTING", global_listing_filename, global_listing_file, global_listing_opened);
    }
    while (!validate_names());

    // Scanner
    TokenType tok;
    do
    {
        tok = scanner();
        fprintf(global_output_file, "token number: %d token type: %s actual token: %s\n", (int)tok, token_type_to_string(tok), token_buffer);
    } while (tok != SCANEOF);

    fprintf(global_listing_file, "%d Lexical Errors.\n", lexical_error_count);

    // Temp files
    init_temp_files();

    // Close everything
    file_close(global_input_file, global_input_opened);
    file_close(global_output_file, global_output_opened);
    file_close(global_listing_file, global_listing_opened);
    file_close(global_temp1_file, global_temp1_opened);
    file_close(global_temp2_file, global_temp2_opened);

    // Delete temp files
    file_delete(global_temp1_file, global_temp1_filename, global_temp1_opened);
    // file_delete(global_temp2_file, global_temp2_filename, global_temp2_opened);

    return 0;
}
