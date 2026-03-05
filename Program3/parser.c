#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "file_util.h"
#include "scanner.h"

void system_goal()
{
    program();
    match(SCANEOF);
}

void program()
{
    match(BEGIN);
    statement_list();
    match(END);
}

int match(TokenType t)
{
    int r = 1;
    if (t != scanner(1))
    {
        r = 0;
    }
    return r;
}

void statement_list()
{
    statement();
    statement_list();
}

int next_token()
{
    return scanner(FALSEOP);
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
        case READ:
    }
}
