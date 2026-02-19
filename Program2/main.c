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

void startup(int argc, char *argv[]);
void wrapup(void);

int main(int argc, char *argv[])
{
    printf("\n");

    open_input_file(argc, argv);
    file_status("INPUT", global_input_filename, global_input_file, global_input_opened);

    do
    {
        open_output_file(argc, argv);
        file_status("OUTPUT", global_output_filename, global_output_file, global_output_opened);

        open_listing_file();
        file_status("LISTING", global_listing_filename, global_listing_file, global_listing_opened);
    }
    while (!validate_names());

    /* Scanner loop — scanner() is called repeatedly until SCANEOF.
     * SCANEOF is still written to the output file before the loop ends.
     * No break used; the do-while condition handles termination. */
    TokenType tok;
    do
    {
        tok = scanner();

        /* Write one token line to the output file:
         * "token number: %d token type: %s actual token: %s\n" */
        fprintf(global_output_file,
                "token number: %d token type: %s actual token: %s\n",
                (int)tok,
                token_type_to_string(tok),
                token_buffer);

    } while (tok != SCANEOF);

    wrapup();

    return 0;
}

/* -------------------------------------------------------
 * wrapup()
 * Finalizes the listing file, handles both temp files per
 * spec, then closes all open file handles.
 * ------------------------------------------------------- */
void wrapup(void)
{
    /* --- Write total lexical error count to listing file --- */
    fprintf(global_listing_file, "%d Lexical Errors.\n", lexical_error_count);

    /* --- Temp File 1: "temp1.tmp" ---
     * Write "The Temp", append its contents to the output file,
     * then delete it. It must NOT exist when the program ends. */
    FILE *temp1 = fopen("temp1.tmp", "w");
    if (temp1 != NULL)
    {
        fprintf(temp1, "The Temp\n");
        fclose(temp1);

        /* Reopen for reading to append into output file */
        temp1 = fopen("temp1.tmp", "r");
        if (temp1 != NULL)
        {
            char ch;
            /* Read temp1 character by character and write to output */
            while ((ch = (char)fgetc(temp1)) != EOF)
            {
                fputc(ch, global_output_file);
            }
            fclose(temp1);
        }

        /* Delete temp1 — it should not exist after program ends */
        remove("temp1.tmp");
    }

    /* --- Temp File 2: "temp2.tmp" ---
     * Write "Loop Information" then close.
     * The remove() call exists below but is commented out so the
     * file PERSISTS when the program ends (per assignment spec). */
    FILE *temp2 = fopen("temp2.tmp", "w");
    if (temp2 != NULL)
    {
        fprintf(temp2, "Loop Information\n");
        fclose(temp2);
    }
    /* remove("temp2.tmp"); */  /* Intentionally left commented out */

    file_close(global_input_file);
    file_close(global_output_file);
    file_close(global_listing_file);
}
