#ifndef PARSER_H
#define PARSER_H
 
#include <stdio.h>
#include "scanner.h"

extern int syntax_error_count;

void system_goal(void);
int match(TokenType expected);
TokenType next_token(void);
 
#endif
