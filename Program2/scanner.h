#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

// Each token type gets a numeric value
typedef enum
{
    BEGIN,          // 0  - "begin"
    END,            // 1  - "end"
    READ,           // 2  - "read"
    WRITE,          // 3  - "write"
    ID,             // 4  - variable
    IF,             // 5  - "if"
    THEN,           // 6  - "then"
    ELSE,           // 7  - "else"
    SEMICOLON,      // 8  - ";"
    COMMA,          // 9  - ","
    ASSIGNOP,       // 10 - ":="
    ENDIF,          // 11 - "endif"
    WHILE,          // 12 - "while"
    SCANEOF,        // 13 - end of file
    ERROR,          // 14 - lexical error
    INTLITERAL,     // integer literal
    FALSEOP,        // "false"
    TRUEOP,         // "true"
    NULLOP,         // "null"
    LPAREN,         // "("
    RPAREN,         // ")"
    PLUSOP,         // "+"
    MINUSOP,        // "-"
    MULTOP,         // "*"
    DIVOP,          // "/"
    NOTOP,          // "!"
    LESSOP,         // "<"
    LESSEQUALOP,    // "<="
    GREATEROP,      // ">"
    GREATEREQUALOP, // ">="
    EQUALOP,        // "="
    NOTEQUALOP,     // "<>"
    ANDOP,          // "and"
    OROP,           // "or"
    ENDWHILE        // "endwhile"
} TokenType;

#define TOKEN_BUFFER_SIZE 256

// Global scanner state
extern char    token_buffer[TOKEN_BUFFER_SIZE];
extern TokenType current_token;
extern char    line_buffer[512];
extern int     line_number;
extern int     lexical_error_count;

TokenType scanner(void);

const char *token_type_to_string(TokenType t);

#endif
