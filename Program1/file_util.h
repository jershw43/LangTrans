#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdio.h>

/* Constants */
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
int validate_names(void);
void generate_extensions(void);
void backup_output(void);
void file_close(void);

#endif
