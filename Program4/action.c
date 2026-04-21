#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <stdarg.h>
#include "file_util.h"
#include "action.h"

#define MAX_STACK   1024
#define MAX_SYMBOLS  256
#define BUF_SIZE    262144
#define COND_BUF_SIZE 4096

// ── Expression stack (ExprRec) ────────────────────────────────────────────────

static ExprRec expr_stack[MAX_STACK];
static int expr_top_index = -1;

void expr_push(const char *s)
{
    if (expr_top_index < MAX_STACK - 1)
    {
        expr_top_index++;
        expr_stack[expr_top_index].kind = EXPR_TEMP;
        strncpy(expr_stack[expr_top_index].name, s,
                sizeof(expr_stack[expr_top_index].name) - 1);
        expr_stack[expr_top_index].name[127] = '\0';
    }
}

static void expr_push_rec(expr_type kind, const char *s)
{
    if (expr_top_index < MAX_STACK - 1)
    {
        expr_top_index++;
        expr_stack[expr_top_index].kind = kind;
        strncpy(expr_stack[expr_top_index].name, s,
                sizeof(expr_stack[expr_top_index].name) - 1);
        expr_stack[expr_top_index].name[127] = '\0';
    }
}

static ExprRec expr_pop_rec(void)
{
    ExprRec empty;
    empty.kind = EXPR_LITERAL;
    strcpy(empty.name, "0");
    if (expr_top_index >= 0)
        return expr_stack[expr_top_index--];
    return empty;
}

char *expr_pop(void)
{
    if (expr_top_index >= 0)
        return expr_stack[expr_top_index--].name;
    return NULL;
}

char *expr_top(void)
{
    if (expr_top_index >= 0)
        return expr_stack[expr_top_index].name;
    return NULL;
}

int expr_depth(void)
{
    return expr_top_index + 1;
}

// ── Operator stack ────────────────────────────────────────────────────────────

static char *op_stack[MAX_STACK];
static int op_top = -1;

static void push_op(const char *op)
{
    op_stack[++op_top] = strdup(op);
}

static char *pop_op(void)
{
    return op_stack[op_top--];
}

// ── Output buffer + condition capture buffer ──────────────────────────────────

static char body_buf[BUF_SIZE];
static int  buf_pos = 0;

static char cond_buf[COND_BUF_SIZE];
static int  cond_buf_pos  = 0;
static int  in_cond_capture = 0;

static void append(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if (in_cond_capture)
        cond_buf_pos += vsnprintf(cond_buf + cond_buf_pos,
                                  COND_BUF_SIZE - cond_buf_pos, fmt, args);
    else
        buf_pos += vsnprintf(body_buf + buf_pos,
                             BUF_SIZE - buf_pos, fmt, args);
    va_end(args);
}

// ── Temp variables ────────────────────────────────────────────────────────────

static int temp_count = 0;

static char *new_temp(void)
{
    char *buf = malloc(16);
    sprintf(buf, "t%d", temp_count++);
    return buf;
}

// ── Symbol table ──────────────────────────────────────────────────────────────

static char *sym_table[MAX_SYMBOLS];
static int   sym_count = 0;

int act_lookup(char *s)
{
    int i;
    for (i = 0; i < sym_count; i++)
    {
        if (strcmp(sym_table[i], s) == 0)
            return 1;
    }
    return 0;
}

void act_enter(char *s)
{
    if (!act_lookup(s) && sym_count < MAX_SYMBOLS)
        sym_table[sym_count++] = strdup(s);
}

void act_check_id(char *s)
{
    if (!act_lookup(s))
        fprintf(g_listing_file,
                "Semantic Error: variable '%s' used before assignment.\n", s);
}

// ── Assignment target ─────────────────────────────────────────────────────────

static char current_lhs[128];

// ── Lifecycle ─────────────────────────────────────────────────────────────────

void act_init(void)
{
    buf_pos       = 0;
    temp_count    = 0;
    sym_count     = 0;
    cond_buf_pos  = 0;
    in_cond_capture = 0;
}

void act_start(void)
{
    time_t now = time(NULL);
    char *dt = ctime(&now);

    fprintf(g_output_file,
        "// C program of %s\n"
        "// Current Date and Time:\n"
        "// %s"
        "#include <stdio.h>\n"
        "int main()\n"
        "{\n",
        g_input_filename,
        dt
    );
}

void act_finish(void)
{
    int i;

    for (i = 0; i < sym_count; i++)
        fprintf(g_output_file, "    int %s;\n", sym_table[i]);

    for (i = 0; i < temp_count; i++)
        fprintf(g_output_file, "    int t%d;\n", i);

    if (sym_count > 0 || temp_count > 0)
        fprintf(g_output_file, "\n");

    fprintf(g_output_file, "%s", body_buf);
    fprintf(g_output_file, "\n    return 0;\n}\n");
}

// ── Expression handling ───────────────────────────────────────────────────────

void act_process_id(const char *id)
{
    act_check_id((char *)id);
    act_enter((char *)id);
    expr_push_rec(EXPR_ID, id);
}

void act_process_literal(const char *lit)
{
    expr_push_rec(EXPR_LITERAL, lit);
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

void act_gen_infix(void)
{
    ExprRec right = expr_pop_rec();
    ExprRec left  = expr_pop_rec();
    char   *op    = pop_op();
    char   *tmp   = new_temp();

    append("    %s = %s %s %s;\n", tmp, left.name, op, right.name);
    expr_push_rec(EXPR_TEMP, tmp);
}

void act_gen_condition(void)
{
    ExprRec top = expr_pop_rec();
    expr_push_rec(EXPR_CONDITION, top.name);
}

// ── Statements ────────────────────────────────────────────────────────────────

void act_start_assign(const char *id)
{
    act_enter((char *)id);
    strcpy(current_lhs, id);
}

void act_assign(void)
{
    ExprRec rhs = expr_pop_rec();
    append("    %s = %s;\n", current_lhs, rhs.name);
}

void act_read_id(const char *id)
{
    act_enter((char *)id);
    append("    scanf(\"%%d\", &%s);\n", id);
}

void act_write_expr(void)
{
    ExprRec e = expr_pop_rec();
    append("    printf(\"%%d\\n\", %s);\n", e.name);
}

// ── Control flow ──────────────────────────────────────────────────────────────

void act_if_start(void)
{
    ExprRec cond = expr_pop_rec();
    append("    if (%s) {\n", cond.name);
}

void act_else(void)
{
    append("    } else {\n");
}

void act_endif(void)
{
    append("    }\n");
}

void act_open_temp(void)
{
    cond_buf_pos    = 0;
    cond_buf[0]     = '\0';
    in_cond_capture = 1;
}

void act_write_tmp(void)
{
    in_cond_capture = 0;
    if (g_temp1_file != NULL)
    {
        fprintf(g_temp1_file, "%s", cond_buf);
        fflush(g_temp1_file);
    }
}

void act_while_start(void)
{
    ExprRec cond = expr_pop_rec();

    buf_pos += snprintf(body_buf + buf_pos, BUF_SIZE - buf_pos,
                        "%s", cond_buf);

    append("    while (%s) {\n", cond.name);
}

void act_endwhile(void)
{
    buf_pos += snprintf(body_buf + buf_pos, BUF_SIZE - buf_pos,
                        "%s", cond_buf);

    append("    }\n");

    cond_buf[0]  = '\0';
    cond_buf_pos = 0;
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void act_generate(char *s[5])
{
    if (s[3] != NULL)
        append("    %s = %s %s %s;\n", s[0], s[1], s[2], s[3]);
    else
        append("    %s = %s;\n", s[0], s[1]);
}

char *act_get_temp(void)
{
    char *buf = malloc(16);
    sprintf(buf, "t%d", temp_count - 1);
    return buf;
}