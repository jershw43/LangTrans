#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include "file_util.h"
#include "scanner.h"
#include "parser.h"
#include "action.h"

#define MAX_STACK 1024
#define MAX_SYMBOLS 256
#define BODY_BUF_SIZE 262144
#define COND_BUF_SIZE 4096

// Expression record
static expr_rec expr_stack[MAX_STACK];
static int expr_top_index = -1;

void expr_push(const char *s)
{
    if (expr_top_index < MAX_STACK - 1)
    {
        expr_top_index++;
        expr_stack[expr_top_index].kind = EXPR_TEMP;
        strncpy(expr_stack[expr_top_index].name, s, sizeof(expr_stack[expr_top_index].name) - 1);
        expr_stack[expr_top_index].name[127] = '\0';
    }
}

static void expr_push_rec(expr_type kind, const char *s)
{
    if (expr_top_index < MAX_STACK - 1)
    {
        expr_top_index++;
        expr_stack[expr_top_index].kind = kind;
        strncpy(expr_stack[expr_top_index].name, s, sizeof(expr_stack[expr_top_index].name) - 1);
        expr_stack[expr_top_index].name[127] = '\0';
    }
}

static expr_rec expr_pop_rec(void)
{
    expr_rec empty;
    empty.kind = EXPR_LITERAL;
    strcpy(empty.name, "0");
    return (expr_top_index >= 0) ? expr_stack[expr_top_index--] : empty;
}

char *expr_pop(void)
{
    return (expr_top_index >= 0) ? expr_stack[expr_top_index--].name : NULL;
}

char *expr_top(void)
{
    return (expr_top_index >= 0) ? expr_stack[expr_top_index].name : NULL;
}

int expr_depth(void)
{
    return expr_top_index + 1;
}

// Operator record
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

// Output + condition capture buffer
static char body_buf[BODY_BUF_SIZE];
static int buf_pos = 0;

static char cond_buf[COND_BUF_SIZE];
static int cond_buf_pos = 0;
static int in_cond_capture = 0;

static void append(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if (in_cond_capture)
        cond_buf_pos += vsnprintf(cond_buf + cond_buf_pos, COND_BUF_SIZE - cond_buf_pos, fmt, args);
    else
        buf_pos += vsnprintf(body_buf + buf_pos, BODY_BUF_SIZE - buf_pos, fmt, args);
    va_end(args);
}

// Temp variables
static int temp_count = 0;

static char *new_temp(void)
{
    char *buf = malloc(16);
    sprintf(buf, "Temp%d", temp_count++ + 1);
    return buf;
}

char *get_temp(void)
{
    char *buf = malloc(16);
    sprintf(buf, "Temp%d", temp_count);
    return buf;
}

// Symbol table
static char *sym_table[MAX_SYMBOLS];
static int sym_count = 0;

static void to_uppercase(char *s)
{
    for (int i = 0; s[i] != '\0'; i++)
        s[i] = (char)toupper((unsigned char)s[i]);
}

int lookup(char *s)
{
    int i;
    int result = 0;
    char upper_s[128];
    strncpy(upper_s, s, sizeof(upper_s) - 1);
    upper_s[sizeof(upper_s) - 1] = '\0';
    to_uppercase(upper_s);
    for (i = 0; i < sym_count; i++)
    {
        if (strcmp(sym_table[i], upper_s) == 0)
            result = 1;
    }
    return result;
}

void enter(char *s)
{
    char upper_s[128];
    strncpy(upper_s, s, sizeof(upper_s) - 1);
    upper_s[sizeof(upper_s) - 1] = '\0';
    to_uppercase(upper_s);
    if (!lookup(s) && sym_count < MAX_SYMBOLS)
        sym_table[sym_count++] = strdup(upper_s);
}

void check_id(char *s)
{
    if (!lookup(s))
        fprintf(g_listing_file, "Semantic Error: variable '%s' used before assignment.\n", s);
}

// Assignment target
static char current_lhs[128];

// Generator
void start(void)
{
    time_t now = time(NULL);
    char *dt = ctime(&now);

    fprintf(g_output_file,
        "/*\n"
        "C program of %s\n"
        "%s"
        "*/\n"
        "#include <stdio.h>\n"
        "int main()\n"
        "{\n",
        g_input_filename,
        dt
    );
}

void generate(char *s[5])
{
    if (s[3] != NULL)
        append("    %s = %s %s %s;\n", s[0], s[1], s[2], s[3]);
    else
        append("    %s = %s;\n", s[0], s[1]);
}

void finish(void)
{
    // Declare variables from symbol table (output file)
    for (int i = 0; i < sym_count; i++)
        fprintf(g_output_file, "    int %s;\n", sym_table[i]);

    // Declare temp variables (output file)
    for (int i = 0; i < temp_count; i++)
        fprintf(g_output_file, "    int Temp%d;\n", i + 1);

    // Write body statements + error status (temp file)
    fprintf(g_temp1_file, "%s", body_buf);
    fprintf(g_temp1_file, "    return 0;\n}\n");
    if (lexical_error_count == 0 && syntax_error_count == 0)
        fprintf(g_temp1_file, "/* Program compiled without errors. */\n");
    else
        fprintf(g_temp1_file, "/* Program compiled with errors. */\n");

    // Stitch output and temp files together
    char c;
    rewind(g_temp1_file);
    while ((c = fgetc(g_temp1_file)) != EOF)
        fputc(c, g_output_file);
}

// Expression handling
void process_id(const char *id)
{
    char upper_id[128];
    strncpy(upper_id, id, sizeof(upper_id) - 1);
    upper_id[sizeof(upper_id) - 1] = '\0';
    to_uppercase(upper_id);
    check_id((char *)id);
    enter((char *)id);
    expr_push_rec(EXPR_ID, upper_id);
}

void process_literal(const char *lit)
{
    expr_push_rec(EXPR_LITERAL, lit);
}

void process_op(const char *op)
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

void gen_infix(void)
{
    expr_rec right = expr_pop_rec();
    expr_rec left = expr_pop_rec();
    char *op = pop_op();
    char *tmp = new_temp();
    append("    %s = %s %s %s;\n", tmp, left.name, op, right.name);
    expr_push_rec(EXPR_TEMP, tmp);
}

void gen_condition(void)
{
    expr_rec top = expr_pop_rec();
    expr_push_rec(EXPR_CONDITION, top.name);
}

// Statements
void act_start_assign(const char *id)
{
    char upper_id[128];
    strncpy(upper_id, id, sizeof(upper_id) - 1);
    upper_id[sizeof(upper_id) - 1] = '\0';
    to_uppercase(upper_id);
    enter((char *)id);
    strcpy(current_lhs, upper_id);
}

void act_assign(void)
{
    expr_rec rhs = expr_pop_rec();
    append("    %s = %s;\n", current_lhs, rhs.name);
}

void act_read_id(const char *id)
{
    char upper_id[128];
    strncpy(upper_id, id, sizeof(upper_id) - 1);
    upper_id[sizeof(upper_id) - 1] = '\0';
    to_uppercase(upper_id);
    enter((char *)id);
    append("    scanf(\"%%d\", &%s);\n", upper_id);
}

void act_write_expr(void)
{
    expr_rec e = expr_pop_rec();
    append("    printf(\"%%d\\n\", %s);\n", e.name);
}

void act_if_start(void)
{
    expr_rec cond = expr_pop_rec();
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
    cond_buf_pos = 0;
    cond_buf[0] = '\0';
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
    expr_rec cond = expr_pop_rec();
    buf_pos += snprintf(body_buf + buf_pos, BODY_BUF_SIZE - buf_pos, "%s", cond_buf);
    append("    while (%s) {\n", cond.name);
}

void act_endwhile(void)
{
    buf_pos += snprintf(body_buf + buf_pos, BODY_BUF_SIZE - buf_pos, "%s", cond_buf);
    append("    }\n");
    cond_buf[0]  = '\0';
    cond_buf_pos = 0;
}
