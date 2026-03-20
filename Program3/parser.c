#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "file_util.h"
#include "scanner.h"
#include "parser.h"

int syntax_error_count = 0;

static char stmt_buffer[1024] = "";

static void program(void);
static void statement_list(void);
static void statement(void);
static void id_list(void);
static void expr_list(void);
static void expression(void);
static void term(void);
static void factor(void);
static void condition(void);
static void c_expression(void);
static void c_term(void);
static void c_factor(void);
static void c_primary(void);
static void add_op(void);
static void mult_op(void);
static void rel_op(void);
static void if_tail(void);

// Print buffer to output file and reset it
static void print_statement(void)
{
    fprintf(g_output_file, "Statement: %s\n", stmt_buffer);
    stmt_buffer[0] = '\0';
}

// Consume next token, print expected/actual, handle mismatch
int match(TokenType expected)
{
    TokenType actual = scanner();
    fprintf(g_output_file, "Expected Token: %s Actual Token: %s \n", token_type_to_string(expected), token_buffer);
    if (actual != expected)
    {
        // Syntax error
        syntax_error_count++;
        fprintf(g_listing_file, "Syntax Error on line %d: Expected %s but found %s\n", line_number, token_type_to_string(expected), token_buffer);

        // Error recovery
        if (expected != SEMICOLON && expected != SCANEOF)
        {
            while (actual != SEMICOLON && actual != SCANEOF)
            {
                actual = scanner();
            }
        }
        stmt_buffer[0] = '\0';
        return 0;
    }

    // Append token (uppercased)
    {
        int i;
        size_t len = strlen(stmt_buffer);
        for (i = 0; token_buffer[i] != '\0' && len < sizeof(stmt_buffer) - 2; i++, len++)
        {
            stmt_buffer[len] = (char) toupper((unsigned char)token_buffer[i]);
        }
        stmt_buffer[len] = '\0';
    }

    // When a semicolon is matched, print and flush the statement
    if (expected == SEMICOLON)
    {
        print_statement();
    }

    return 1;
}

// Peek at next token without consuming it
TokenType next_token(void)
{
    return peek_token();
}

void system_goal(void)
{
    program();
    match(SCANEOF);
    print_statement();
}

static void program(void)
{
    match(BEGIN);
    print_statement();
    statement_list();
    match(END);
    print_statement();
}

static void statement_list(void)
{
    TokenType t = next_token();
    while (t != END && t != ENDIF && t != ELSE && t != ENDWHILE && t != SCANEOF)
    {
        statement();
        t = next_token();
    }
}

static void statement(void)
{
    TokenType t = next_token();
    switch (t)
    {
        case ID:
            match(ID);
            match(ASSIGNOP);
            expression();
            match(SEMICOLON);
            break;
        case READ:
            match(READ);
            match(LPAREN);
            id_list();
            match(RPAREN);
            match(SEMICOLON);
            break;
        case WRITE:
            match(WRITE);
            match(LPAREN);
            expr_list();
            match(RPAREN);
            match(SEMICOLON);
            break;
        case IF:
            match(IF);
            match(LPAREN);
            condition();
            match(RPAREN);
            if (next_token() == THEN)
            {
                match(THEN);
                print_statement();
            }
            else
            {
                print_statement();
                match(THEN);
                print_statement();
            }
            statement_list();
            if_tail();
            break;
        case WHILE:
            match(WHILE);
            match(LPAREN);
            condition();
            match(RPAREN);
            print_statement();
            statement_list();
            match(ENDWHILE);
            print_statement();
            break;
        default:
            syntax_error_count++;
            fprintf(g_listing_file, "Syntax Error on line %d: Unexpected token %s\n", line_number, token_type_to_string(t));
            {
                TokenType skip = scanner();
                while (skip != SEMICOLON && skip != SCANEOF)
                {
                    skip = scanner();
                }
            }
            stmt_buffer[0] = '\0';
            break;
    }
}

static void if_tail(void)
{
    TokenType t = next_token();
    if (t == ELSE)
    {
        match(ELSE);
        print_statement();
        statement_list();
        match(ENDIF);
        print_statement();
    }
    else
    {
        match(ENDIF);
        print_statement();
    }
}

static void id_list(void)
{
    match(ID);
    while (next_token() == COMMA)
    {
        match(COMMA);
        match(ID);
    }
}

static void expr_list(void)
{
    expression();
    while (next_token() == COMMA)
    {
        match(COMMA);
        expression();
    }
}

static void expression(void)
{
    TokenType t;
    term();
    t = next_token();
    while (t == PLUSOP || t == MINUSOP)
    {
        add_op();
        term();
        t = next_token();
    }
}

static void term(void)
{
    TokenType t;
    factor();
    t = next_token();
    while (t == MULTOP || t == DIVOP)
    {
        mult_op();
        factor();
        t = next_token();
    }
}

static void factor(void)
{
    TokenType t = next_token();
    switch (t)
    {
        case LPAREN:
            match(LPAREN);
            expression();
            match(RPAREN);
            break;
        case MINUSOP:
            match(MINUSOP);
            factor();
            break;
        case ID:
            match(ID);
            break;
        case INTLITERAL:
            match(INTLITERAL);
            break;
        default:
            syntax_error_count++;
            fprintf(g_listing_file, "Syntax Error on line %d: Expected factor but found %s\n", line_number, token_type_to_string(t));
            break;
    }
}

static void condition(void)
{
    TokenType t = next_token();

    if (t == LPAREN)
    {
        c_expression();
    }
    else if (t == NOTOP)
    {
        match(NOTOP);
        match(LPAREN);
        condition();
        match(RPAREN);
    }
    else if (t == MINUSOP)
    {
        match(MINUSOP);
        condition();
    }
    else
    {
        c_expression();
    }

    t = next_token();
    while (t == ANDOP || t == OROP)
    {
        scanner(); /* consume and/or */
        {
            int i;
            size_t len = strlen(stmt_buffer);
            for (i = 0; token_buffer[i] != '\0' && len < sizeof(stmt_buffer)-2; i++, len++)
                stmt_buffer[len] = (char)toupper((unsigned char)token_buffer[i]);
            stmt_buffer[len] = '\0';
        }
        c_expression();
        t = next_token();
    }
}

static void c_expression(void)
{
    TokenType t;
    c_term();
    t = next_token();
    while (t == LESSOP || t == LESSEQUALOP || t == GREATEROP ||
        t == GREATEREQUALOP || t == EQUALOP || t == NOTEQUALOP)
    {
        rel_op();
        c_term();
        t = next_token();
    }
}

static void c_term(void)
{
    TokenType t;
    c_factor();
    t = next_token();
    while (t == PLUSOP || t == MINUSOP)
    {
        add_op();
        c_factor();
        t = next_token();
    }
}

static void c_factor(void)
{
    TokenType t;
    c_primary();
    t = next_token();
    while (t == MULTOP || t == DIVOP)
    {
        mult_op();
        c_primary();
        t = next_token();
    }
}

static void c_primary(void)
{
    TokenType t = next_token();
    switch (t)
    {
        case LPAREN:
            match(LPAREN);
            c_expression();
            match(RPAREN);
            break;
        case MINUSOP:
            match(MINUSOP);
            c_primary();
            break;
        case NOTOP:
            match(NOTOP);
            match(LPAREN);
            c_expression();
            match(RPAREN);
            break;
        case ID:
            match(ID);
            break;
        case INTLITERAL:
            match(INTLITERAL);
            break;
        case FALSEOP:
            match(FALSEOP);
            break;
        case TRUEOP:
            match(TRUEOP);
            break;
        case NULLOP:
            match(NULLOP);
            break;
        default:
            syntax_error_count++;
            fprintf(g_listing_file, "Syntax Error on line %d: Expected primary but found %s\n", line_number, token_type_to_string(t));
            break;
    }
}

static void add_op(void)
{
    TokenType t = next_token();
    if (t == PLUSOP) match(PLUSOP);
    else if (t == MINUSOP) match(MINUSOP);
}

static void mult_op(void)
{
    TokenType t = next_token();
    if (t == MULTOP) match(MULTOP);
    else if (t == DIVOP) match(DIVOP);
}

static void rel_op(void)
{
    TokenType t = next_token();
    switch (t)
    {
        case LESSOP:
            match(LESSOP);
            break;
        case LESSEQUALOP:
            match(LESSEQUALOP);
            break;
        case GREATEROP:
            match(GREATEROP);
            break;
        case GREATEREQUALOP:
            match(GREATEREQUALOP);
            break;
        case EQUALOP:
            match(EQUALOP);
            break;
        case NOTEQUALOP:
            match(NOTEQUALOP);
            break;
        default:
            syntax_error_count++;
            fprintf(g_listing_file, "Syntax Error on line %d: Expected relational operator but found %s\n", line_number, token_type_to_string(t));
            break;
    }
}
