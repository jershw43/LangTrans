#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "file_util.h"
#include "scanner.h"

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
    TokenType result;

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
    if      (strcasecmp(token_buffer, "begin")    == 0) { result = BEGIN;    }
    else if (strcasecmp(token_buffer, "end")      == 0) { result = END;      }
    else if (strcasecmp(token_buffer, "read")     == 0) { result = READ;     }
    else if (strcasecmp(token_buffer, "write")    == 0) { result = WRITE;    }
    else if (strcasecmp(token_buffer, "if")       == 0) { result = IF;       }
    else if (strcasecmp(token_buffer, "then")     == 0) { result = THEN;     }
    else if (strcasecmp(token_buffer, "else")     == 0) { result = ELSE;     }
    else if (strcasecmp(token_buffer, "endif")    == 0) { result = ENDIF;    }
    else if (strcasecmp(token_buffer, "while")    == 0) { result = WHILE;    }
    else if (strcasecmp(token_buffer, "endwhile") == 0) { result = ENDWHILE; }
    else if (strcasecmp(token_buffer, "false")    == 0) { result = FALSEOP;  }
    else if (strcasecmp(token_buffer, "true")     == 0) { result = TRUEOP;   }
    else if (strcasecmp(token_buffer, "null")     == 0) { result = NULLOP;   }
    else if (strcasecmp(token_buffer, "and")      == 0) { result = ANDOP;    }
    else if (strcasecmp(token_buffer, "or")       == 0) { result = OROP;     }
    else                                                 { result = ID;       }

    return result;
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
    TokenType result;

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
                result = ASSIGNOP;
            }
            else
            {
                if (n != EOF)
                {
                    lookahead = (unsigned char)n;
                }
                lexical_error_count = lexical_error_count + 1;
                fprintf(g_listing_file, "Error. ':' not recognized in line %d.\n", line_number + 1);
                result = ERROR;
            }
            break;

        case '<':
            n = next_char();
            if (n == '=')
            {
                token_buffer[1] = '=';
                token_buffer[2] = '\0';
                result = LESSEQUALOP;
            }
            else if (n == '>')
            {
                token_buffer[1] = '>';
                token_buffer[2] = '\0';
                result = NOTEQUALOP;
            }
            else
            {
                if (n != EOF)
                {
                    lookahead = (unsigned char)n;
                }
                result = LESSOP;
            }
            break;

        case '>':
            n = next_char();
            if (n == '=')
            {
                token_buffer[1] = '=';
                token_buffer[2] = '\0';
                result = GREATEREQUALOP;
            }
            else
            {
                if (n != EOF)
                {
                    lookahead = (unsigned char)n;
                }
                result = GREATEROP;
            }
            break;

        case '+':
            result = PLUSOP;
            break;

        case '-':
            n = next_char();
            if (n == '-')
            {
                /* comment start; consume until end-of-line then start over */
                skip_comment();
                result = scanner();
            }
            else
            {
                if (n != EOF)
                {
                    lookahead = (unsigned char)n;
                }
                result = MINUSOP;
            }
            break;

        case '*':
            result = MULTOP;
            break;

        case '/':
            result = DIVOP;
            break;

        case '!':
            result = NOTOP;
            break;

        case '=':
            result = EQUALOP;
            break;

        case '(':
            result = LPAREN;
            break;

        case ')':
            result = RPAREN;
            break;

        case ';':
            result = SEMICOLON;
            break;

        case ',':
            result = COMMA;
            break;

        default:
            lexical_error_count = lexical_error_count + 1;
            fprintf(g_listing_file, "Error. '%c' not recognized in line %d.\n", c, line_number + 1);
            result = ERROR;
            break;
    }

    return result;
}

const char *token_type_to_string(TokenType t)
{
    const char *result;

    switch (t) {
        case BEGIN:         result = "BEGIN";         break;
        case END:           result = "END";           break;
        case READ:          result = "READ";          break;
        case WRITE:         result = "WRITE";         break;
        case ID:            result = "ID";            break;
        case IF:            result = "IF";            break;
        case THEN:          result = "THEN";          break;
        case ELSE:          result = "ELSE";          break;
        case SEMICOLON:     result = "SEMICOLON";     break;
        case COMMA:         result = "COMMA";         break;
        case ASSIGNOP:      result = "ASSIGNOP";      break;
        case ENDIF:         result = "ENDIF";         break;
        case WHILE:         result = "WHILE";         break;
        case SCANEOF:       result = "SCANEOF";       break;
        case ERROR:         result = "ERROR";         break;
        case INTLITERAL:    result = "INTLITERAL";    break;
        case FALSEOP:       result = "FALSEOP";       break;
        case TRUEOP:        result = "TRUEOP";        break;
        case NULLOP:        result = "NULLOP";        break;
        case LPAREN:        result = "LPAREN";        break;
        case RPAREN:        result = "RPAREN";        break;
        case PLUSOP:        result = "PLUSOP";        break;
        case MINUSOP:       result = "MINUSOP";       break;
        case MULTOP:        result = "MULTOP";        break;
        case DIVOP:         result = "DIVOP";         break;
        case NOTOP:         result = "NOTOP";         break;
        case LESSOP:        result = "LESSOP";        break;
        case LESSEQUALOP:   result = "LESSEQUALOP";   break;
        case GREATEROP:     result = "GREATEROP";     break;
        case GREATEREQUALOP:result = "GREATEREQUALOP";break;
        case EQUALOP:       result = "EQUALOP";       break;
        case NOTEQUALOP:    result = "NOTEQUALOP";    break;
        case ANDOP:         result = "ANDOP";         break;
        case OROP:          result = "OROP";          break;
        case ENDWHILE:      result = "ENDWHILE";      break;
        default:            result = "UNKNOWN";       break;
    }

    return result;
}

TokenType scanner(void)
{
    char c;

    skip_whitespace(&c);

    // End of file
    if (c == (char)EOF)
    {
        current_token = SCANEOF;
    }
    else if (isalpha((unsigned char)c))
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