#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include "action.h"

/* =========================
   Expression stack
   ========================= */
#define MAX_STACK 1024

static char *expr_stack[MAX_STACK];
static int expr_top_index = -1;

void expr_push(const char *s)
{
    if (expr_top_index < MAX_STACK - 1)
    {
        expr_stack[++expr_top_index] = strdup(s);
    }
}

char *expr_pop()
{
    if (expr_top_index >= 0)
        return expr_stack[expr_top_index--];
    return NULL;
}

char *expr_top()
{
    if (expr_top_index >= 0)
        return expr_stack[expr_top_index];
    return NULL;
}

int expr_depth()
{
    return expr_top_index + 1;
}

/* =========================
   Operator stack
   ========================= */
static char *op_stack[MAX_STACK];
static int op_top = -1;

static void push_op(const char *op)
{
    op_stack[++op_top] = strdup(op);
}

static char *pop_op()
{
    return op_stack[op_top--];
}

/* =========================
   Output buffer
   ========================= */
#define BUF_SIZE 262144

static char body_buf[BUF_SIZE];
static int buf_pos = 0;

static void append(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    buf_pos += vsnprintf(body_buf + buf_pos, BUF_SIZE - buf_pos, fmt, args);
    va_end(args);
}

/* =========================
   Temp variables
   ========================= */
static int temp_count = 0;

static char *new_temp()
{
    char *buf = malloc(16);
    sprintf(buf, "t%d", temp_count++);
    return buf;
}

/* =========================
   Assignment target
   ========================= */
static char current_lhs[128];

/* =========================
   Public API
   ========================= */

void act_init()
{
    buf_pos = 0;
    temp_count = 0;
}

void act_start()
{
    /* nothing needed */
}

void act_finish()
{
    extern FILE *g_output_file;

    fprintf(g_output_file, "#include <stdio.h>\n\n");
    fprintf(g_output_file, "int main() {\n");

    /* Declare temps */
    if (temp_count > 0)
    {
        fprintf(g_output_file, "    int ");
        for (int i = 0; i < temp_count; i++)
        {
            fprintf(g_output_file, "t%d", i);
            if (i != temp_count - 1)
                fprintf(g_output_file, ", ");
        }
        fprintf(g_output_file, ";\n\n");
    }

    fprintf(g_output_file, "%s", body_buf);

    fprintf(g_output_file, "\n    return 0;\n}\n");
}

/* =========================
   Expression handling
   ========================= */

void act_process_id(const char *id)
{
    expr_push(id);
}

void act_process_literal(const char *lit)
{
    expr_push(lit);
}

void act_process_op(const char *op)
{
    if (strcmp(op, "=") == 0)
        push_op("==");
    else if (strcmp(op, "<>") == 0)
        push_op("!=");
    else if (strcasecmp(op, "and") == 0)
        push_op("&&");
    else if (strcasecmp(op, "or") == 0)
        push_op("||");
    else
        push_op(op);
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

/* =========================
   Statements
   ========================= */

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

/* =========================
   Control flow
   ========================= */

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