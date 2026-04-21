#ifndef ACTION_H
#define ACTION_H

void act_init(void);
void act_start(void);
void act_finish(void);

// Expression handling
void act_process_id(const char *id);
void act_process_literal(const char *lit);
void act_process_op(const char *op);
void act_gen_infix(void);

// Statements
void act_start_assign(const char *id);
void act_assign(void);
void act_read_id(const char *id);
void act_write_expr(void);
void act_if_start(void);
void act_else(void);
void act_endif(void);
void act_while_start(void);
void act_endwhile(void);

// Expression stack utilities
void expr_push(const char *s);
char *expr_pop(void);
char *expr_top(void);
int expr_depth(void);

// NEEDS IMPLEMENTING: proper expression records
typedef enum {
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
	FALSEOP,        // "false"
    TRUEOP,         // "true"
    NULLOP          // "null"
} op_expr;

typedef enum {
	ID,
	LITERAL,
	TEMP,
	CONDITION
} expr_type;

typedef enum {
	TYPE,
	EXPR
} expr_rec;

// NEEDS IMPLEMENTING: some more action routines
void act_open_temp(void);
void act_write_tmp(void);
void act_gen_condition(void);

// NEEDS IMPLEMENTING: additional helper functions from Lesson 17
// not exactly sure where these fit in, but they could probably replace some of the statement functions
int act_lookup(char *s);
void act_enter(char *s);
void act_check_id(char *s);
void act_generate(char *s[5]);
char *act_get_temp(void);

#endif
