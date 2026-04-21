#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "file_util.h"
#include "scanner.h"
#include "parser.h"
#include "action.h"

int syntax_error_count = 0;
char stmt_buffer[1024] = "";

void print_statement(void)
{
    fprintf(g_listing_file, "Statement: %s\n", stmt_buffer);
    stmt_buffer[0] = '\0';
}

int match(TokenType expected)
{
    int result = 1;
    TokenType actual = scanner();

    fprintf(g_listing_file,
        "Expected Token: %s Actual Token: %s\n",
        token_type_to_string(expected), token_buffer);

    if (actual != expected)
    {
        syntax_error_count++;
        fprintf(g_listing_file,
            "Syntax Error on line %d: Expected %s but found %s\n",
            line_number,
            token_type_to_string(expected),
            token_buffer);

        while (actual != SEMICOLON && actual != SCANEOF)
            actual = scanner();

        stmt_buffer[0] = '\0';
        result = 0;
    }
    else
    {
        int i;
        size_t len = strlen(stmt_buffer);

        for (i = 0; token_buffer[i] != '\0' && len < sizeof(stmt_buffer) - 2; i++, len++)
            stmt_buffer[len] = (char)toupper((unsigned char)token_buffer[i]);

        stmt_buffer[len] = '\0';

        if (expected == SEMICOLON)
            print_statement();
    }

    return result;
}

TokenType next_token(void)
{
    return peek_token();
}

void system_goal(void)
{
    act_init();          
    program();
    match(SCANEOF);
    act_finish();        
}

void program(void)
{
    act_start();
    match(BEGIN);
    statement_list();
    match(END);
}

void statement_list(void)
{
    TokenType t = next_token();
    while (t != END && t != ENDIF && t != ELSE && t != ENDWHILE && t != SCANEOF)
    {
        statement();
        t = next_token();
    }
}

void statement(void)
{
    TokenType t = next_token();

    char temp[128];

    switch (t)
    {
        case ID:
            strcpy(temp, token_buffer);
            match(ID);

            act_start_assign(temp);   

            match(ASSIGNOP);
            expression();

            act_assign();             

            match(SEMICOLON);
            break;

        case READ:
            match(READ);
            match(LPAREN);

            strcpy(temp, token_buffer);
            match(ID);
            act_read_id(temp);        

            while (next_token() == COMMA)
            {
                match(COMMA);
                strcpy(temp, token_buffer);
                match(ID);
                act_read_id(temp);    
            }

            match(RPAREN);
            match(SEMICOLON);
            break;

        case WRITE:
            match(WRITE);
            match(LPAREN);

            expression();
            act_write_expr();         

            while (next_token() == COMMA)
            {
                match(COMMA);
                expression();
                act_write_expr();     
            }

            match(RPAREN);
            match(SEMICOLON);
            break;

        case IF:
            match(IF);
            match(LPAREN);

            condition();
            act_if_start();           

            match(RPAREN);
            match(THEN);

            statement_list();

            if (next_token() == ELSE)
            {
                match(ELSE);
                act_else();           
                statement_list();
            }

            match(ENDIF);
            act_endif();              
            break;

        case WHILE:
            match(WHILE);
            match(LPAREN);

            condition();
            act_while_start();        

            match(RPAREN);

            statement_list();

            match(ENDWHILE);
            act_endwhile();           
            break;

        default:
            syntax_error_count++;
            fprintf(g_listing_file,
                "Syntax Error on line %d: Unexpected token %s\n",
                line_number,
                token_type_to_string(t));
            break;
    }
}

// Expressions
void expression(void)
{
    term();

    while (next_token() == PLUSOP || next_token() == MINUSOP)
    {
        char op[32];
        strcpy(op, token_buffer);
        add_op();
        act_process_op(op);       

        term();
        act_gen_infix();          
    }
}

void term(void)
{
    factor();

    while (next_token() == MULTOP || next_token() == DIVOP)
    {
        char op[32];
        strcpy(op, token_buffer);
        mult_op();
        act_process_op(op);       

        factor();
        act_gen_infix();          
    }
}

void factor(void)
{
    TokenType t = next_token();
    char temp[128];

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
            strcpy(temp, token_buffer);
            match(ID);
            act_process_id(temp);     
            break;

        case INTLITERAL:
            strcpy(temp, token_buffer);
            match(INTLITERAL);
            act_process_literal(temp); 
            break;

        default:
            syntax_error_count++;
            fprintf(g_listing_file,
                "Syntax Error on line %d: Expected factor but found %s\n",
                line_number,
                token_type_to_string(t));
            break;
    }
}

// Conditions
void condition(void)
{
    c_expression();

    while (next_token() == ANDOP || next_token() == OROP)
    {
        char op[32];
        strcpy(op, token_buffer);
        match(next_token());

        act_process_op(op);       

        c_expression();
        act_gen_infix();          
    }
}

void c_expression(void)
{
    c_term();

    while (next_token() == LESSOP || next_token() == LESSEQUALOP ||
           next_token() == GREATEROP || next_token() == GREATEREQUALOP ||
           next_token() == EQUALOP || next_token() == NOTEQUALOP)
    {
        char op[32];
        strcpy(op, token_buffer);
        rel_op();

        act_process_op(op);       

        c_term();
        act_gen_infix();          
    }
}

void c_term(void)
{
    c_factor();

    while (next_token() == PLUSOP || next_token() == MINUSOP)
    {
        char op[32];
        strcpy(op, token_buffer);
        add_op();

        act_process_op(op);       

        c_factor();
        act_gen_infix();          
    }
}

void c_factor(void)
{
    c_primary();

    while (next_token() == MULTOP || next_token() == DIVOP)
    {
        char op[32];
        strcpy(op, token_buffer);
        mult_op();

        act_process_op(op);       

        c_primary();
        act_gen_infix();          
    }
}

void c_primary(void)
{
    TokenType t = next_token();
    char temp[128];

    switch (t)
    {
        case LPAREN:
            match(LPAREN);
            c_expression();
            match(RPAREN);
            break;

        case ID:
            strcpy(temp, token_buffer);
            match(ID);
            act_process_id(temp);
            break;

        case INTLITERAL:
            strcpy(temp, token_buffer);
            match(INTLITERAL);
            act_process_literal(temp);
            break;

        case TRUEOP:
            match(TRUEOP);
            act_process_literal("1");
            break;

        case FALSEOP:
            match(FALSEOP);
            act_process_literal("0");
            break;

        case NULLOP:
            match(NULLOP);
            act_process_literal("0");
            break;

        default:
            syntax_error_count++;
            fprintf(g_listing_file,
                "Syntax Error on line %d: Expected primary but found %s\n",
                line_number,
                token_type_to_string(t));
            break;
    }
}

// Operators
void add_op(void)
{
    if (next_token() == PLUSOP) match(PLUSOP);
    else match(MINUSOP);
}

void mult_op(void)
{
    if (next_token() == MULTOP) match(MULTOP);
    else match(DIVOP);
}

void rel_op(void)
{
    match(next_token());
}
