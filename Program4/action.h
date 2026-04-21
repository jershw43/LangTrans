#ifndef ACTION_H
#define ACTION_H

// Generation
void act_init(void);
void act_start(void);
void act_finish(void);

// Expression handling
void act_process_id(const char *id);
void act_process_literal(const char *lit);
void act_process_op(const char *op);
void act_gen_infix(void);
void act_gen_condition(void);

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
void act_write_temp(void);

// NEEDS IMPLEMENTING: additional helper functions from Lesson 17
// not exactly sure where these fit in, but they could probably replace some of the statement functions
int act_lookup(char *s);
void act_enter(char *s);
void act_check_id(char *s);
void act_generate(char *s[5]);
char *act_get_temp(void);

#endif
