#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "file_util.h"
#include "scanner.h"

int match(TokenType t)
{
    int r = 1;
    if (t != scanner())
    {
        r = 0;
    }
    return r;
}

int next_token()
{
    return scanner();
}

void program()
{
    match(BEGIN);
    statement_list();
    match(END);
}

void statement_list()
{
    statement();
    match(LBRACE);
    statement_list();
    match(RBRACE);
}

void statement()
{
    TokenType t;
    t = next_token();
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
            match(THEN);
            statement_list();
            if_tail();
            break;
        case WHILE:
            match(WHILE);
            match(LPAREN);
            condition();
            match(RPAREN);
            statement_list();
            match(ENDWHILE);
            break;
    }
}

void if_tail()
{
    TokenType t;
    t = next_token();
    switch (t)
    {
        case ELSE:
            match(ELSE);
            statement_list();
            match(ENDIF);
            break;
        case ENDIF:
            match(ENDIF);
            break;
    }
}

void id_list()
{
    match(ID);
    match(LBRACE);
    match(COMMA);
    id_list();
    match(RBRACE);
}

void expr_list()
{
    expression();
    match(LBRACE);
    match(COMMA);
    expr_list();
    match(RBRACE);
}

void expression()
{
    term();
    match(LBRACE);
    add_op();
    term();
    match(RBRACE);
}

void term()
{
    factor();
    match(LBRACE);
    mult_op();
    factor();
    match(RBRACE);
}

void factor()
{
    TokenType t;
    t = next_token();
    switch (t)
    {
        case LPAREN:
            match(LPAREN);
            expression();
            match(RPAREN);
            break;
        case MINUSOP:
            match(MINUSOP);
            break;
        case ID:
            match(ID);
            break;
        case INTLITERAL:
            match(INTLITERAL);
            break;
    }
}

void condition()
{
    TokenType t;
    t = next_token();
    switch (t)
    {
        default:
            c_expression();
            match(LBRACE);
            logical_op();
            c_expression();
            match(RBRACE);
            break;
        case LPAREN:
            match(LPAREN);
            condition();
            match(RPAREN);
            break;
        case MINUSOP:
            match(MINUSOP);
            condition();
            break;
    }
}

void c_expression()
{
    c_term();
    match(LBRACE);
    rel_op();
    c_term();
    match(RBRACE);
}

void c_term()
{
    c_factor();
    match(LBRACE);
    add_op();
    c_term();
    match(RBRACE);
}

void c_factor()
{
    c_primary();
    match(LBRACE);
    mult_op();
    c_term();
    match(RBRACE);
}

void c_primary() {
    TokenType t;
    t = next_token();
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
            c_primary();
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
    }
}

void add_op()
{
    TokenType t;
    t = next_token();
    switch (t)
    {
        case PLUSOP:
            match(PLUSOP);
            break;
        case MINUSOP:
            match(MINUSOP);
            break;
    }
}

void mult_op()
{
    TokenType t;
    t = next_token();
    switch (t)
    {
        case MULTOP:
            match(MULTOP);
            break;
        case DIVOP:
            match(DIVOP);
            break;
    }
}

void rel_op()
{
    TokenType t;
    t = next_token();
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
    }
}

void logical_op()
{
    TokenType t;
    t = next_token();
    switch (t)
    {
        case ANDOP:
            match(ANDOP);
            break;
        case OROP:
            match(OROP);
            break;
    }
}

void system_goal()
{
    program();
    match(SCANEOF);
}
