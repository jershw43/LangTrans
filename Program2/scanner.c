#include "scanner.h"
#include "file_util.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Global variables
char token_buffer[TOKEN_BUFFER_SIZE];
TokenType current_token;
char line_buffer[512];
int line_number = 0;
int lexical_error_count = 0;
static int lookahead = -1;

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

static void skip_comment(void)
{
    char c;

    do
    {
        c = next_char();
    } while (c != '\n' && c != EOF);
}

static TokenType scan_identifier_or_keyword(char first_char)
{
    int  idx = 0;
    char c;

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

    // The character that ended the identifier is NOT part of this token
    // Save it as the lookahead for the next scanner call
    if (c != (char)EOF)
    {
        lookahead = (unsigned char)c;
    }

    // Null-terminate
    token_buffer[idx] = '\0';

    // Reserved-word comparison
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
        // Not a keyword
        return ID;
    }
}

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

    // The character that ended the integer literal is NOT part of this token
    // Save it as lookahead
    if (c != (char)EOF)
    {
        lookahead = (unsigned char)c;
    }

    // Null-terminate
    token_buffer[idx] = '\0';

    return INTLITERAL;
}

static TokenType scan_operator(char c)
{
    char n;

    // Put first character into the token buffer
    token_buffer[0] = c;
    token_buffer[1] = '\0';

    switch (c)
    {
        case ':':
            n = next_char();
            if (n == '=')
            {
                token_buffer[1] = '=';
                token_buffer[2] = '\0';
                return ASSIGNOP;
            }
            if (n != EOF)
            {
                lookahead = (unsigned char)n;
            }
            lexical_error_count = lexical_error_count + 1;
            fprintf(g_listing_file, "Error. ':' not recognized in line %d.\n", line_number + 1);
            return ERROR;

        case '<':
            n = next_char();
            if (n == '=')
            {
                token_buffer[1] = '=';
                token_buffer[2] = '\0';
                return LESSEQUALOP;
            }
            if (n == '>')
            {
                token_buffer[1] = '>';
                token_buffer[2] = '\0';
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
                token_buffer[1] = '=';
                token_buffer[2] = '\0';
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
                /* comment start; consume until end-of-line then start over */
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
            fprintf(g_listing_file, "Error. '%c' not recognized in line %d.\n", c, line_number + 1);
            return ERROR;
    }
}

const char *token_type_to_string(TokenType t)
{
    switch (t) {
        case BEGIN: return "BEGIN";
        case END: return "END";
        case READ: return "READ";
        case WRITE: return "WRITE";
        case ID: return "ID";
        case IF: return "IF";
        case THEN: return "THEN";
        case ELSE: return "ELSE";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case ASSIGNOP: return "ASSIGNOP";
        case ENDIF: return "ENDIF";
        case WHILE: return "WHILE";
        case SCANEOF: return "SCANEOF";
        case ERROR: return "ERROR";
        case INTLITERAL: return "INTLITERAL";
        case FALSEOP: return "FALSEOP";
        case TRUEOP: return "TRUEOP";
        case NULLOP: return "NULLOP";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case PLUSOP: return "PLUSOP";
        case MINUSOP: return "MINUSOP";
        case MULTOP: return "MULTOP";
        case DIVOP: return "DIVOP";
        case NOTOP: return "NOTOP";
        case LESSOP: return "LESSOP";
        case LESSEQUALOP: return "LESSEQUALOP";
        case GREATEROP: return "GREATEROP";
        case GREATEREQUALOP: return "GREATEREQUALOP";
        case EQUALOP: return "EQUALOP";
        case NOTEQUALOP: return "NOTEQUALOP";
        case ANDOP: return "ANDOP";
        case OROP: return "OROP";
        case ENDWHILE: return "ENDWHILE";
        default: return "UNKNOWN";
    }
}

TokenType scanner(void)
{
    char c;

    skip_whitespace(&c);

    // End of file
    if (c == (char)EOF)
    {
        current_token = SCANEOF;
        return current_token;
    }

    // Determine token type
    if (isalpha((unsigned char)c))
    {
        current_token = scan_identifier_or_keyword(c);
    }
    else if (isdigit((unsigned char)c))
    {
        current_token = scan_integer(c);
    }
    else
    {
        current_token = scan_operator(c);
    }

    return current_token;
}
