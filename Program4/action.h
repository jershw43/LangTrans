#ifndef ACTION_H
#define ACTION_H

// Records
typedef enum {
    EXPR_ID,
    EXPR_LITERAL,
    EXPR_TEMP,
    EXPR_CONDITION
} expr_type;

typedef struct {
    expr_type kind;
    char name[128];
} expr_rec;

// Generator
void start(void);
void generate(char *s[5]);
void finish(void);

// Expression handling
void process_id(const char *id);
void process_literal(const char *lit);
void process_op(const char *op);
void gen_infix(void);
void gen_condition(void);

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
void act_open_temp(void);
void act_write_tmp(void);

#endif
