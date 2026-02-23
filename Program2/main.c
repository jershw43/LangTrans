/*
 * main.c
 * CSMC 4180 - Language Translation
 * Scanner Program #2
 * Group #: [GROUP NUMBER]
 * Members: [NAMES & EMAILS]
 *
 * Purpose: Entry point. Handles startup (file opening), drives the
 *          scanner loop, builds output and listing files, and runs
 *          the wrapup routine.
 *
 * NOTE: DO NOT use break, goto, or exit anywhere in this file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"
#include "scanner.h"

/* -------------------------------------------------------
 * FUNCTION PROTOTYPES
 * ------------------------------------------------------- */
void startup(int argc, char *argv[]);
void wrapup(void);

/* -------------------------------------------------------
 * main()
 *
 * Flow:
 *   1. Call startup()
 *   2. Loop: call scanner() until SCANEOF is returned
 *       - Each iteration write one line to the OUTPUT file:
 *         "token number: %d token type: %s actual token: %s\n"
 *         using (int)current_token, token_type_to_string(), token_buffer
 *   3. After loop ends, call wrapup()
 * ------------------------------------------------------- */
int main(int argc, char *argv[])
{
    /* TODO: Call startup() */

    /* TODO: Scanner loop
     *   TokenType tok;
     *   do {
     *       tok = scanner();
     *       -- write token line to global_output_file --
     *   } while (tok != SCANEOF);
     *
     * NOTE: Use do-while, not break. The SCANEOF token itself
     *       should still be written to the output file before stopping.
     */

    /* TODO: Call wrapup() */

    return 0;
}

/* -------------------------------------------------------
 * startup()
 * Initializes everything before scanning begins.
 *
 * Steps:
 *   1. open_input_file(argc, argv)   -- from file_util
 *   2. open_output_file(argc, argv)  -- from file_util
 *   3. open_listing_file()           -- from file_util
 *   4. validate_names() loop         -- ensure no filename conflicts
 *   5. Initialize scanner globals:
 *        line_number = 0
 *        lexical_error_count = 0
 *        token_buffer[0] = '\0'
 *        line_buffer[0]  = '\0'
 *
 * TODO: Implement startup()
 * ------------------------------------------------------- */
void startup(int argc, char *argv[])
{
    /* TODO */
}

/* -------------------------------------------------------
 * wrapup()
 * Cleanup after scanning is complete.
 *
 * Steps:
 *   1. Write the total lexical error count to the listing file:
 *        "%d Lexical Errors.\n", lexical_error_count
 *
 *   2. TEMP FILE 1 ("temp1.tmp"):
 *       - Open temp1.tmp for writing
 *       - Write "The Temp\n" into it
 *       - Close it
 *       - Append its contents to global_output_file
 *       - Delete temp1.tmp (it should NOT exist after program ends)
 *       TODO: Implement append-then-delete logic here
 *
 *   3. TEMP FILE 2 ("temp2.tmp"):
 *       - Open temp2.tmp for writing
 *       - Write "Loop Information\n" into it
 *       - Close it
 *       - The delete call should exist but remain COMMENTED OUT
 *         so temp2.tmp persists when the program ends
 *       TODO: Write the file, leave remove() call commented out
 *
 *   4. Call file_close() to close all open file handles
 *
 * NOTE: DO NOT use break, goto, or exit.
 * ------------------------------------------------------- */
void wrapup(void)
{
    /* TODO */
}
