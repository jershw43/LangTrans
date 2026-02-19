/*
 * scanner.h
 * CSMC 4180 - Language Translation
 * Scanner Program #2
 * Group #: [GROUP NUMBER]
 * Members: [NAMES & EMAILS]
 *
 * Purpose: Header for scanner module. Defines token types,
 *          token buffer, and scanner function prototype.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

/* -------------------------------------------------------
 * TOKEN ENUM
 * Each token type gets a numeric value.
 * The order here determines the "token number" printed
 * in the output file.
 * TODO: Confirm numeric ordering matches assignment spec.
 *       Example from spec: BEGIN=0, END=1, ... ID=4, etc.
 * ------------------------------------------------------- */
typedef enum {
    BEGIN,        /* 0  - "begin"    */
    END,          /* 1  - "end"      */
    READ,         /* 2  - "read"     */
    WRITE,        /* 3  - "write"    */
    ID,           /* 4  - variable   */
    IF,           /* 5  - "if"       */
    THEN,         /* 6  - "then"     */
    ELSE,         /* 7  - "else"     */
    SEMICOLON,    /* 8  - ";"        */
    COMMA,        /* 9  - ","        */
    ASSIGNOP,     /* 10 - ":="       */
    ENDIF,        /* 11 - "endif"    */
    WHILE,        /* 12 - "while"    */
    SCANEOF,      /* 13 - end of file */
    ERROR,        /* 14 - lexical error */
    INTLITERAL,   /* integer literal  */
    FALSEOP,      /* "false"          */
    TRUEOP,       /* "true"           */
    NULLOP,       /* "null"           */
    LPAREN,       /* "("              */
    RPAREN,       /* ")"              */
    PLUSOP,       /* "+"              */
    MINUSOP,      /* "-"              */
    MULTOP,       /* "*"              */
    DIVOP,        /* "/"              */
    NOTOP,        /* "!"              */
    LESSOP,       /* "<"              */
    LESSEQUALOP,  /* "<="             */
    GREATEROP,    /* ">"              */
    GREATEREQUALOP, /* ">="           */
    EQUALOP,      /* "="              */
    NOTEQUALOP,   /* "<>"             */
    ANDOP,        /* "and"            */
    OROP,         /* "or"             */
    ENDWHILE      /* "endwhile"       */
    /* TODO: Verify all tokens are present and numbered correctly */
} TokenType;

/* -------------------------------------------------------
 * TOKEN BUFFER SIZE
 * Max length of any single token's text
 * ------------------------------------------------------- */
#define TOKEN_BUFFER_SIZE 256

/* -------------------------------------------------------
 * GLOBAL SCANNER STATE (declared here, defined in scanner.c)
 *
 * token_buffer : holds the actual text of the current token
 * current_token: holds the enum value of the current token
 * line_buffer  : holds the full current source line (for listing)
 * line_number  : current line being scanned
 * lexical_error_count : running total of errors found
 * ------------------------------------------------------- */
extern char    token_buffer[TOKEN_BUFFER_SIZE];
extern TokenType current_token;
extern char    line_buffer[512];
extern int     line_number;
extern int     lexical_error_count;

/* -------------------------------------------------------
 * FUNCTION PROTOTYPE
 *
 * scanner() - reads from global_input_file, sets
 *             current_token and token_buffer, and
 *             returns the TokenType found.
 * ------------------------------------------------------- */
TokenType scanner(void);

/* -------------------------------------------------------
 * HELPER: token_type_to_string()
 * Converts a TokenType enum value to its name string
 * (e.g., BEGIN -> "BEGIN") for output file formatting.
 * TODO: Implement in scanner.c
 * ------------------------------------------------------- */
const char *token_type_to_string(TokenType t);

#endif /* SCANNER_H */
