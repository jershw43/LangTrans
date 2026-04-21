#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <stdarg.h>
#include "file_util.h"
#include "action.h"

#define MAX_RECS 1024
#define BUF_SIZE 262144

// Records
static char *op_rec[MAX_RECS];
static int op_top = -1;
static char *expr_rec[MAX_RECS];
static int expr_top_index = -1;

// Symbol table
static char *vars[MAX_RECS];
static int var_count = 0;

// Output buffer/generation
static char body_buf[BUF_SIZE];
static int buf_pos = 0;
static char current_lhs[128];
static int temp_count = 0;
int loop_start_pos;

// Operator record
static void push_op(const char *op)
{
    op_rec[++op_top] = strdup(op);
}

static char *pop_op()
{
    return op_rec[op_top--];
}

// Expression record
void expr_push(const char *s)
{
    if (expr_top_index < MAX_RECS - 1)
    {
        expr_rec[++expr_top_index] = strdup(s);
    }
}

char *expr_pop()
{
    if (expr_top_index >= 0)
        return expr_rec[expr_top_index--];
    return NULL;
}

char *expr_top()
{
    if (expr_top_index >= 0)
        return expr_rec[expr_top_index];
    return NULL;
}

int expr_depth()
{
    return expr_top_index + 1;
}

// Output buffer
static void append(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    buf_pos += vsnprintf(body_buf + buf_pos, BUF_SIZE - buf_pos, fmt, args);
    va_end(args);
}

// Temp variables
static char *new_temp()
{
    char *buf = malloc(16);
    sprintf(buf, "Temp%d", temp_count++);
    return buf;
}

// Symbol table
static int var_exists(const char *id)
{
    for (int i = 0; i < var_count; i++)
        if (strcmp(vars[i], id) == 0)
            return 1;
    return 0;
}

static void add_var(const char *id)
{
    if (!var_exists(id))
        vars[var_count++] = strdup(id);
}

// Generator
void act_init()
{
    buf_pos = 0;
    temp_count = 0;
}

void act_start()
{
    time_t now = time(NULL);
    char *dt = ctime(&now);

    fprintf(g_output_file,
        "/*\n"
        "C program of %s\n"
        "%s"
        "*/\n\n"
        "#include <stdio.h>\n"
        "int main()\n"
        "{\n",
        g_input_filename,
        dt
    );
}

void act_finish()
{
    // Declare variables
    for (int i = 0; i < var_count; i++)
        fprintf(g_output_file, "    int %s;\n", vars[i]);

    // Declare temps
    for (int i = 0; i < temp_count; i++)
        fprintf(g_output_file, "    int Temp%d;\n", i);

    // Print statements
    fprintf(g_output_file, "%s", body_buf);
    fprintf(g_output_file, "    return 0;\n}\n\n");

    fprintf(g_listing_file, "PROGRAM COMPILED WITH NO ERRORS.\n");
    fprintf(g_output_file, "/* PROGRAM COMPILED WITH NO ERRORS. */\n");
}

// Expression handling
void act_process_id(const char *id)
{
    add_var(id);
    expr_push(id);
}

void act_process_literal(const char *lit)
{
    expr_push(lit);
}

void act_process_op(const char *op)
{
    if      (strcasecmp(op, "true") == 0)  expr_push("1");
    else if (strcasecmp(op, "false") == 0) expr_push("0");
    else if (strcasecmp(op, "null") == 0)  expr_push("0");
    else if (strcmp(op, "=") == 0)         push_op("==");
    else if (strcmp(op, "<>") == 0)        push_op("!=");
    else if (strcasecmp(op, "and") == 0)   push_op("&&");
    else if (strcasecmp(op, "or") == 0)    push_op("||");
    else                                   push_op(op);
}

void act_gen_infix()
{
    char *right = expr_pop();
    char *left = expr_pop();
    char *op = pop_op();
    char *temp = new_temp();
    append("    %s = %s %s %s;\n", temp, left, op, right);
    expr_push(temp);
}

void act_gen_condition()
{
    char *right = expr_pop();
    char *left  = expr_pop();
    char *op    = pop_op();
    char *temp = new_temp();
    append("    %s = %s %s %s;\n", temp, left, op, right);
    expr_push(temp);
}

// Statements
void act_start_assign(const char *id)
{
    strcpy(current_lhs, id);
}

void act_assign()
{
    char *rhs = expr_pop();
    append("    %s = %s;\n", current_lhs, rhs);
}

void act_read_id(const char *id)
{
    append("    scanf(\"%%d\", &%s);\n", id);
}

void act_write_expr()
{
    char *expr = expr_pop();
    append("    printf(\"%%d\\n\", %s);\n", expr);
}

void act_if_start()
{
    char *cond = expr_pop();
    append("    if (%s) {\n", cond);
}

void act_else()
{
    append("    } else {\n");
}

void act_endif()
{
    append("    }\n");
}

void act_while_start()
{
    char *cond = expr_pop();
    append("    while (%s) {\n", cond);
}

void act_endwhile()
{
    append("    }\n");
}

void act_open_temp()
{
    loop_start_pos = buf_pos;
}

void act_write_temp()
{
    append("%.*s", buf_pos - loop_start_pos, body_buf + loop_start_pos);
}
