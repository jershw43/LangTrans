/*
    parser.c - Recursive descent parser
    Program 3, CMSC 4180
*/
 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "file_util.h"
#include "scanner.h"
#include "parser.h"
 
/* counts */
int syntax_error_count = 0;
 
/* statement buffer - accumulates tokens to print the completed statement */
static char stmt_buffer[1024] = "";
 
/* forward declarations of all parse functions */
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
 
/* -----------------------------------------------------------------------
 * print_statement - print stmt_buffer to output file and reset it
 * ----------------------------------------------------------------------- */
static void print_statement(void)
{
    fprintf(g_output_file, "Statement: %s\n", stmt_buffer);
    stmt_buffer[0] = '\0';
}
 
/* -----------------------------------------------------------------------
 * match - consume next token; print expected/actual; handle mismatch
 * ----------------------------------------------------------------------- */
int match(TokenType expected)
{
    TokenType actual = scanner();
 
    /* print expected and actual to output file */
    fprintf(g_output_file, "Expected Token: %s Actual Token: %s \n",
            token_type_to_string(expected), token_buffer);
 
    if (actual != expected)
    {
        /* syntax error */
        syntax_error_count++;
        fprintf(g_listing_file,
                "Syntax Error on line %d: Expected %s but found %s\n",
                line_number, token_type_to_string(expected), token_buffer);
 
        /* error recovery: skip to next semicolon then return */
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
 
    /* append token (uppercased) to statement buffer */
    {
        int i;
        size_t len = strlen(stmt_buffer);
        for (i = 0; token_buffer[i] != '\0' && len < sizeof(stmt_buffer) - 2; i++, len++)
        {
            stmt_buffer[len] = (char)toupper((unsigned char)token_buffer[i]);
        }
        stmt_buffer[len] = '\0';
    }
 
    /* when a semicolon is matched, print and flush the statement */
    if (expected == SEMICOLON)
    {
        print_statement();
    }
 
    return 1;
}
 
/* -----------------------------------------------------------------------
 * next_token - peek at next token without consuming it
 * ----------------------------------------------------------------------- */
TokenType next_token(void)
{
    return peek_token();
}
 
/* -----------------------------------------------------------------------
 * system_goal : <program> SCANEOF
 * ----------------------------------------------------------------------- */
void system_goal(void)
{
    program();
    match(SCANEOF);
    print_statement();   /* "Statement: EOF" */
}
 
/* -----------------------------------------------------------------------
 * program : BEGIN <statement_list> END
 * ----------------------------------------------------------------------- */
static void program(void)
{
    match(BEGIN);
    print_statement();   /* "Statement: BEGIN" */
    statement_list();
    match(END);
    print_statement();   /* "Statement: END" */
}
 
/* -----------------------------------------------------------------------
 * statement_list : <statement> { <statement_list> }
 * ----------------------------------------------------------------------- */
static void statement_list(void)
{
    TokenType t;
 
    t = next_token();
 
    /* statement_list ends when we see END, ENDIF, ELSE, ENDWHILE, or EOF */
    while (t != END && t != ENDIF && t != ELSE && t != ENDWHILE && t != SCANEOF)
    {
        statement();
        t = next_token();
    }
}
 
/* -----------------------------------------------------------------------
 * statement
 * ----------------------------------------------------------------------- */
static void statement(void)
{
    TokenType t = next_token();
 
    switch (t)
    {
        /* ID := <expression> ; */
        case ID:
            match(ID);
            match(ASSIGNOP);
            expression();
            match(SEMICOLON);
            break;
 
        /* READ ( <id_list> ) ; */
        case READ:
            match(READ);
            match(LPAREN);
            id_list();
            match(RPAREN);
            match(SEMICOLON);
            break;
 
        /* WRITE ( <expr_list> ) ; */
        case WRITE:
            match(WRITE);
            match(LPAREN);
            expr_list();
            match(RPAREN);
            match(SEMICOLON);
            break;
 
        /* IF ( <condition> ) THEN <statement_list> <if_tail> */
        case IF:
            match(IF);
            match(LPAREN);
            condition();
            match(RPAREN);
            if (next_token() == THEN)
            {
                match(THEN);
                print_statement();   /* "Statement: IF(...)THEN" */
            }
            else
            {
                print_statement();   /* "Statement: IF(...)" */
                match(THEN);
                print_statement();   /* "Statement: THEN" */
            }
            statement_list();
            if_tail();
            break;
 
        /* WHILE ( <condition> ) { <statement_list> } ENDWHILE */
        case WHILE:
            match(WHILE);
            match(LPAREN);
            condition();
            match(RPAREN);
            print_statement();   /* "Statement: WHILE(...)" */
            statement_list();
            match(ENDWHILE);
            print_statement();   /* "Statement: ENDWHILE" */
            break;
 
        default:
            /* unexpected token */
            syntax_error_count++;
            fprintf(g_listing_file,
                    "Syntax Error on line %d: Unexpected token %s\n",
                    line_number, token_type_to_string(t));
            /* consume the bad token and recover */
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
 
/* -----------------------------------------------------------------------
 * if_tail : ELSE <statement_list> ENDIF  |  ENDIF
 * ----------------------------------------------------------------------- */
static void if_tail(void)
{
    TokenType t = next_token();
 
    if (t == ELSE)
    {
        match(ELSE);
        print_statement();   /* "Statement: ELSE" */
        statement_list();
        match(ENDIF);
        print_statement();   /* "Statement: ENDIF" */
    }
    else
    {
        match(ENDIF);
        print_statement();   /* "Statement: ENDIF" */
    }
}
 
/* -----------------------------------------------------------------------
 * id_list : ID { , <id_list> }
 * ----------------------------------------------------------------------- */
static void id_list(void)
{
    match(ID);
    while (next_token() == COMMA)
    {
        match(COMMA);
        match(ID);
    }
}
 
/* -----------------------------------------------------------------------
 * expr_list : <expression> { , <expr_list> }
 * ----------------------------------------------------------------------- */
static void expr_list(void)
{
    expression();
    while (next_token() == COMMA)
    {
        match(COMMA);
        expression();
    }
}
 
/* -----------------------------------------------------------------------
 * expression : <term> { <add_op> <term> }
 * ----------------------------------------------------------------------- */
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
 
/* -----------------------------------------------------------------------
 * term : <factor> { <mult_op> <factor> }
 * ----------------------------------------------------------------------- */
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
 
/* -----------------------------------------------------------------------
 * factor : ( <expression> ) | - <factor> | ID | INTLITERAL
 * ----------------------------------------------------------------------- */
static void factor(void)
{
    TokenType t = next_token();
 
    if (t == LPAREN)
    {
        match(LPAREN);
        expression();
        match(RPAREN);
    }
    else if (t == MINUSOP)
    {
        match(MINUSOP);
        factor();
    }
    else if (t == ID)
    {
        match(ID);
    }
    else if (t == INTLITERAL)
    {
        match(INTLITERAL);
    }
    else
    {
        syntax_error_count++;
        fprintf(g_listing_file,
                "Syntax Error on line %d: Expected factor but found %s\n",
                line_number, token_type_to_string(t));
    }
}
 
/* -----------------------------------------------------------------------
 * condition : <c_expression> { <logical_op> <c_expression> }
 *           | ( <condition> )
 *           | ! <condition>
 * ----------------------------------------------------------------------- */
static void condition(void)
{
    TokenType t = next_token();
 
    if (t == LPAREN)
    {
        /* could be ( <condition> ) or start of c_expression */
        /* peek two ahead is complex; treat as c_expression path */
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
 
    /* handle logical operators */
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
 
/* -----------------------------------------------------------------------
 * c_expression : <c_term> { <rel_op> <c_term> }
 * ----------------------------------------------------------------------- */
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
 
/* -----------------------------------------------------------------------
 * c_term : <c_factor> { <add_op> <c_term> }
 * ----------------------------------------------------------------------- */
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
 
/* -----------------------------------------------------------------------
 * c_factor : <c_primary> { <mult_op> <c_factor> }
 * ----------------------------------------------------------------------- */
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
 
/* -----------------------------------------------------------------------
 * c_primary : ( <c_expression> ) | - <c_primary> | ! <c_primary>
 *           | ID | INTLITERAL | FALSE | TRUE | NULL
 * ----------------------------------------------------------------------- */
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
            fprintf(g_listing_file,
                    "Syntax Error on line %d: Expected primary but found %s\n",
                    line_number, token_type_to_string(t));
            break;
    }
}
 
/* -----------------------------------------------------------------------
 * add_op : + | -
 * ----------------------------------------------------------------------- */
static void add_op(void)
{
    TokenType t = next_token();
    if (t == PLUSOP)       match(PLUSOP);
    else if (t == MINUSOP) match(MINUSOP);
}
 
/* -----------------------------------------------------------------------
 * mult_op : * | /
 * ----------------------------------------------------------------------- */
static void mult_op(void)
{
    TokenType t = next_token();
    if (t == MULTOP)      match(MULTOP);
    else if (t == DIVOP)  match(DIVOP);
}
 
/* -----------------------------------------------------------------------
 * rel_op : < | <= | > | >= | = | <>
 * ----------------------------------------------------------------------- */
static void rel_op(void)
{
    TokenType t = next_token();
    switch (t)
    {
        case LESSOP:         match(LESSOP);         break;
        case LESSEQUALOP:    match(LESSEQUALOP);    break;
        case GREATEROP:      match(GREATEROP);      break;
        case GREATEREQUALOP: match(GREATEREQUALOP); break;
        case EQUALOP:        match(EQUALOP);        break;
        case NOTEQUALOP:     match(NOTEQUALOP);     break;
        default:
            syntax_error_count++;
            fprintf(g_listing_file,
                    "Syntax Error on line %d: Expected relational operator but found %s\n",
                    line_number, token_type_to_string(t));
            break;
    }
}