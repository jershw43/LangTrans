#ifndef ACTION_H
#define ACTION_H

void act_init();
void act_start();
void act_finish();

/* Expression handling */
void act_process_id(const char *id);
void act_process_literal(const char *lit);
void act_process_op(const char *op);
void act_gen_infix();

/* Statements */
void act_start_assign(const char *id);
void act_assign();

void act_read_id(const char *id);
void act_write_expr();

void act_if_start();
void act_else();
void act_endif();

void act_while_start();
void act_endwhile();

/* Expression stack utilities */
void expr_push(const char *s);
char *expr_pop();
char *expr_top();
int expr_depth();

#endif