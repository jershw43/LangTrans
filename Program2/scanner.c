/*
 * scanner.c
 * CSMC 4180 - Language Translation
 * Scanner Program #2
 * Group #: [GROUP NUMBER]
 * Members: [NAMES & EMAILS]
 *
 * Purpose: Implements the scanner (lexical analyzer). Reads characters
 *          from g_input_file one at a time, builds tokens, and
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
 *   - Pulls next character from g_input_file
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
        c = fgetc(g_input_file);
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
            fprintf(g_listing_file, "%d\t%s\n", line_number, line_buffer);
            line_buffer[0] = '\0';
        }
    }

    return (char)c;
}
//Cam
/* TODO: static void skip_whitespace(char *c)
 *   - Consume whitespace characters, calling next_char()
 *   - Stop when a non-whitespace char is found
 *   - Leave that character in *c for the caller to use
 */

static void skip_comment(void)
{
    char c;

    do
    {
        c = next_char();
    } while (c != '\n' && c != EOF);
}

 //Cam
/* TODO: static TokenType scan_identifier_or_keyword(char first_char)
 *   - Build token_buffer starting with first_char
 *   - Keep reading alphanumeric / underscore chars via next_char()
 *   - When done, compare token_buffer (case-insensitive?) against
 *     all reserved words:
 *       begin, end, read, write, if, then, else, endif,
 *       while, endwhile, false, true, null, and, or
 *   - Return the matching keyword token, or ID if no match
 */

 //Cam
/* TODO: static TokenType scan_integer(char first_char)
 *   - Build token_buffer starting with first_char
 *   - Keep reading digit characters via next_char()
 *   - Return INTLITERAL
 */

static TokenType scan_operator(char c)
{
    char n;

    switch (c)
    {
        case ':':
            n = next_char();
            if (n == '=')
            {
                return ASSIGNOP;
            }
            if (n != EOF)
            {
                lookahead = (unsigned char)n;
            }
            lexical_error_count = lexical_error_count + 1;
            fprintf(global_listing_file, "Lexical Error (line %d): unexpected ':'\n", line_number);
            return ERROR;

        case '<':
            n = next_char();
            if (n == '=')
            {
                return LESSEQUALOP;
            }
            if (n == '>')
            {
                return NOTEQUALOP;
            }
            if (n != EOF)
            {
                lookahead = (unsigned char)n;
            }
            return LESSOP;

        case '>':
            n = next_char();
            if (n == '=')
            {
                return GREATEREQUALOP;
            }
            if (n != EOF)
            {
                lookahead = (unsigned char)n;
            }
            return GREATEROP;

        case '+':
            return PLUSOP;

        case '-':
            n = next_char();
            if (n == '-')
            {
                skip_comment();
                return scanner();
            }
            if (n != EOF)
            {
                lookahead = (unsigned char)n;
            }
            return MINUSOP;

        case '*':
            return MULTOP;

        case '/':
            return DIVOP;

        case '!':
            return NOTOP;

        case '=':
            return EQUALOP;

        case '(':
            return LPAREN;

        case ')':
            return RPAREN;

        case ';':
            return SEMICOLON;

        case ',':
            return COMMA;

        default:
            lexical_error_count = lexical_error_count + 1;
            fprintf(global_listing_file, "Lexical Error (line %d): unexpected character '%c'\n", line_number, c);
            return ERROR;
    }
}

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
