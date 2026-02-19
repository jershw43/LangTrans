/*
 * scanner.c
 * CSMC 4180 - Language Translation
 * Scanner Program #2
 * Group #: [GROUP NUMBER]
 * Members: [NAMES & EMAILS]
 *
 * Purpose: Implements the scanner (lexical analyzer). Reads characters
 *          from global_input_file one at a time, builds tokens, and
 *          returns the identified TokenType to main.
 */

#include "scanner.h"
#include "file_util.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* -------------------------------------------------------
 * GLOBAL VARIABLE DEFINITIONS
 * (declared extern in scanner.h)
 * ------------------------------------------------------- */
char     token_buffer[TOKEN_BUFFER_SIZE];
TokenType current_token;
char     line_buffer[512];
int      line_number        = 0;
int      lexical_error_count = 0;

/* -------------------------------------------------------
 * INTERNAL / STATIC VARIABLES
 * ------------------------------------------------------- */
static int lookahead = -1;
/* TODO: Add a static variable to hold the "lookahead" character
 *       so we don't lose a character after peeking ahead.
 *       Example: static int lookahead = -1;
 */

/* -------------------------------------------------------
 * INTERNAL HELPER PROTOTYPES (not exposed in header)
 * ------------------------------------------------------- */

/* TODO: static char next_char(void)
 *   - Pulls next character from global_input_file
 *   - When a newline is read:
 *       1. Increment line_number
 *       2. Write line_buffer to listing file with line number
 *          Format: "%d\t%s\n", line_number, line_buffer
 *       3. Reset line_buffer to empty
 *   - Append each character into line_buffer as it's read
 *   - Return the character (or EOF sentinel)
 */
static char next_char(void)
{
    int c = 0;
    size_t len = 0;

    if (lookahead != -1)
    {
        c = lookahead;
        lookahead = -1;
    }
    else
    {
        c = fgetc(global_input_file);
    }

    if (c != EOF)
    {
        len = strlen(line_buffer);
        if (len < sizeof(line_buffer) - 1)
        {
            line_buffer[len] = (char)c;
            line_buffer[len + 1] = '\0';
        }

        if (c == '\n')
        {
            size_t line_len = strlen(line_buffer);
            if (line_len > 0 && line_buffer[line_len - 1] == '\n')
            {
                line_buffer[line_len - 1] = '\0';
            }

            line_number = line_number + 1;
            fprintf(global_listing_file, "%d\t%s\n", line_number, line_buffer);
            line_buffer[0] = '\0';
        }
    }

    return (char)c;
}
/* TODO: static void skip_whitespace(char *c)
 *   - Consume whitespace characters, calling next_char()
 *   - Stop when a non-whitespace char is found
 *   - Leave that character in *c for the caller to use
 */

/* TODO: static void skip_comment(void)
 *   - Called when "--" is detected
 *   - Consume all remaining characters on the current line
 *   - Effectively reads until '\n' or EOF
 */

/* TODO: static TokenType scan_identifier_or_keyword(char first_char)
 *   - Build token_buffer starting with first_char
 *   - Keep reading alphanumeric / underscore chars via next_char()
 *   - When done, compare token_buffer (case-insensitive?) against
 *     all reserved words:
 *       begin, end, read, write, if, then, else, endif,
 *       while, endwhile, false, true, null, and, or
 *   - Return the matching keyword token, or ID if no match
 */

/* TODO: static TokenType scan_integer(char first_char)
 *   - Build token_buffer starting with first_char
 *   - Keep reading digit characters via next_char()
 *   - Return INTLITERAL
 */

/* TODO: static TokenType scan_operator(char c)
 *   - Handle single and multi-character operators
 *   - Cases to handle:
 *       ':'  -> peek for '=' -> ASSIGNOP (":=") or ERROR
 *       '<'  -> peek for '=' -> LESSEQUALOP, or '>' -> NOTEQUALOP, else LESSOP
 *       '>'  -> peek for '=' -> GREATEREQUALOP, else GREATEROP
 *       '+'  -> PLUSOP
 *       '-'  -> peek for second '-' -> comment (call skip_comment, re-enter scanner)
 *                otherwise MINUSOP
 *       '*'  -> MULTOP
 *       '/'  -> DIVOP
 *       '!'  -> NOTOP
 *       '='  -> EQUALOP
 *       '('  -> LPAREN
 *       ')'  -> RPAREN
 *       ';'  -> SEMICOLON
 *       ','  -> COMMA
 *       anything else -> ERROR (log to listing file)
 *   - For PEEK operations: read the next char; if it doesn't complete
 *     the two-char token, save it as the lookahead for next call
 */

/* -------------------------------------------------------
 * token_type_to_string()
 * Maps TokenType enum -> string name for output file
 * ------------------------------------------------------- */
const char *token_type_to_string(TokenType t)
{
    /* TODO: Implement with a switch statement or string array
     * Example:
     *   case BEGIN: return "BEGIN";
     *   case END:   return "END";
     *   ...
     *   default:    return "UNKNOWN";
     */
    return "UNKNOWN"; /* placeholder */
}

/* -------------------------------------------------------
 * scanner()
 * Main scanner function called repeatedly from main.
 *
 * Flow:
 *   1. Skip whitespace
 *   2. Check for EOF -> return SCANEOF
 *   3. If letter -> scan_identifier_or_keyword()
 *   4. If digit  -> scan_integer()
 *   5. Otherwise -> scan_operator()
 *   6. Set current_token and return it
 *
 * NOTE: DO NOT use break, goto, or exit anywhere.
 * ------------------------------------------------------- */
TokenType scanner(void)
{
    /* TODO: Implement scanner body following the flow above */

    /* Placeholder so file compiles */
    return SCANEOF;
}
