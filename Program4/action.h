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

typedef enum {
    EXPR_PLUS,
    EXPR_MINUS,
    EXPR_MULT,
    EXPR_DIV,
    EXPR_NOT,
    EXPR_LESS,
    EXPR_LESSEQ,
    EXPR_GREATER,
    EXPR_GREATEREQ,
    EXPR_EQUAL,
    EXPR_NOTEQUAL,
    EXPR_FALSE,
    EXPR_TRUE,
    EXPR_NULL
} op_expr;

typedef enum {
    EXPR_ID,
    EXPR_LITERAL,
    EXPR_TEMP,
    EXPR_CONDITION
} expr_type;

typedef struct {
    expr_type kind;
    char      name[128];
} ExprRec;

void act_open_temp(void);
void act_write_tmp(void);
void act_gen_condition(void);

int act_lookup(char *s);
void act_enter(char *s);
void act_check_id(char *s);
void act_generate(char *s[5]);
char *act_get_temp(void);

#endif