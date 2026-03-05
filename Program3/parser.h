#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "scanner.h"

void system_goal();
void program();
int match(TokenType t);
void statement_list();
int next_token();
void statement();

#endif
