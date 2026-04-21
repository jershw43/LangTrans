#ifndef PARSER_H
#define PARSER_H
 
#include <stdio.h>
#include "scanner.h"
#include "action.h"   

extern int syntax_error_count;
extern char stmt_buffer[1024];  

/* =========================
   Core parsing functions
   ========================= */
void system_goal(void);
int match(TokenType expected);
TokenType next_token(void);

/* =========================
   Grammar productions
   ========================= */
void program(void);
void statement_list(void);
void statement(void);
void if_tail(void);
void id_list(void);
void expr_list(void);
void expression(void);
void term(void);
void factor(void);

/* =========================
   Conditions
   ========================= */
void condition(void);
void c_expression(void);
void c_term(void);
void c_factor(void);
void c_primary(void);

/* =========================
   Operators
   ========================= */
void add_op(void);
void mult_op(void);
void rel_op(void);
 
#endif