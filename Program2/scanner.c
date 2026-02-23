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

/* -------------------------------------------------------
 * INTERNAL HELPER PROTOTYPES (not exposed in header)
 * ------------------------------------------------------- */
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
static void skip_whitespace(char *c)
{
    // Read the first character to start the check
    *c = next_char();

    // Keep reading while the character is whitespace
    while (*c != (char)EOF && isspace((unsigned char)*c))
    {
        *c = next_char();
    }
    // *c now holds the first non-whitespace character (or EOF)
}

/* TODO: static void skip_comment(void)
 *   - Called when "--" is detected
 *   - Consume all remaining characters on the current line
 *   - Effectively reads until '\n' or EOF
 */

//Cam
static TokenType scan_identifier_or_keyword(char first_char)
{
    int  idx = 0;
    char c;

    /* --- Build the token string in token_buffer --- */

    // Store the first character already read by the caller
    token_buffer[idx++] = first_char;

    // Continue reading while we see letters, digits, or '_'
    c = next_char();
    while (c != (char)EOF && (isalnum((unsigned char)c) || c == '_'))
    {
        if (idx < TOKEN_BUFFER_SIZE - 1)
        {
            token_buffer[idx++] = c;
        }
        c = next_char();
    }

    /* The character that ended the identifier is NOT part of this
     * token. Save it as the lookahead for the next scanner call. */
    if (c != (char)EOF)
    {
        lookahead = (unsigned char)c;
    }

    // Null-terminate
    token_buffer[idx] = '\0';

    /* --- Reserved-word comparison (case-insensitive) --- */

    if      (strcasecmp(token_buffer, "begin")    == 0) { return BEGIN;    }
    else if (strcasecmp(token_buffer, "end")      == 0) { return END;      }
    else if (strcasecmp(token_buffer, "read")     == 0) { return READ;     }
    else if (strcasecmp(token_buffer, "write")    == 0) { return WRITE;    }
    else if (strcasecmp(token_buffer, "if")       == 0) { return IF;       }
    else if (strcasecmp(token_buffer, "then")     == 0) { return THEN;     }
    else if (strcasecmp(token_buffer, "else")     == 0) { return ELSE;     }
    else if (strcasecmp(token_buffer, "endif")    == 0) { return ENDIF;    }
    else if (strcasecmp(token_buffer, "while")    == 0) { return WHILE;    }
    else if (strcasecmp(token_buffer, "endwhile") == 0) { return ENDWHILE; }
    else if (strcasecmp(token_buffer, "false")    == 0) { return FALSEOP;  }
    else if (strcasecmp(token_buffer, "true")     == 0) { return TRUEOP;   }
    else if (strcasecmp(token_buffer, "null")     == 0) { return NULLOP;   }
    else if (strcasecmp(token_buffer, "and")      == 0) { return ANDOP;    }
    else if (strcasecmp(token_buffer, "or")       == 0) { return OROP;     }
    else
    {
        /* Not a keyword — it is a user-defined identifier */
        return ID;
    }
}

//Cam
static TokenType scan_integer(char first_char)
{
    int  idx = 0;
    char c;

    // Store the leading digit already supplied by the caller
    token_buffer[idx++] = first_char;

    // Keep reading digits
    c = next_char();
    while (c != (char)EOF && isdigit((unsigned char)c))
    {
        if (idx < TOKEN_BUFFER_SIZE - 1)
        {
            token_buffer[idx++] = c;
        }
        c = next_char();
    }

    /* The character that ended the integer literal is NOT part of
     * this token. Save it as lookahead.                          */
    if (c != (char)EOF)
    {
        lookahead = (unsigned char)c;
    }

    // Null-terminate
    token_buffer[idx] = '\0';

    return INTLITERAL;
}

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