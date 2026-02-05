#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdio.h>

/* Constants */
#define PRINT_RED     "\x1b[31m"
#define PRINT_YELLOW  "\x1b[33m"
#define PRINT_GREEN   "\x1b[32m"
#define PRINT_BLUE    "\x1b[34m"
#define PRINT_CYAN    "\x1b[36m"
#define PRINT_ITALIC  "\x1b[3m"
#define PRINT_RESET   "\x1b[0m"

#define MAX_FILENAME 256
#define IN  ".IN"
#define OUT ".OUT"
#define LIS ".LIS"
#define BAK ".BAK"

/* ---------- GLOBAL VARIABLES ---------- */
extern char global_input_filename[MAX_FILENAME];
extern char global_output_filename[MAX_FILENAME];
extern char global_listing_filename[MAX_FILENAME];

extern FILE *global_input_file;
extern FILE *global_output_file;
extern FILE *global_listing_file;

extern int global_input_opened;
extern int global_output_opened;
extern int global_listing_opened;

/* ---------- FUNCTION PROTOTYPES ---------- */
void open_input_file(int argc, char *argv[]);
void open_output_file(int argc, char *argv[]);
void open_listing_file(void);
void create_temp_files(void);
void validate_names(void);
void generate_extensions(void);
void backup_output(void);
void file_close(void);
void file_status(char* name, char* filename, FILE *file, int is_opened, char* color);

#endif
