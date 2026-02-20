#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdio.h>

// Constants
#define MAX_FILENAME 256
#define IN  ".in"
#define OUT ".out"
#define LIS ".lis"
#define BAK ".bak"
#define TMP ".tmp"

// File variables
extern FILE *g_input_file;
extern FILE *g_output_file;
extern FILE *g_backup_file;
extern FILE *g_listing_file;
extern FILE *g_temp1_file;
extern FILE *g_temp2_file;

extern char g_input_filename[MAX_FILENAME];
extern char g_output_filename[MAX_FILENAME];
extern char g_backup_filename[MAX_FILENAME];
extern char g_listing_filename[MAX_FILENAME];
extern char g_temp1_filename[MAX_FILENAME];
extern char g_temp2_filename[MAX_FILENAME];

extern int g_input_opened;
extern int g_output_opened;
extern int g_backup_opened;
extern int g_listing_opened;
extern int g_temp1_opened;
extern int g_temp2_opened;

// Prototypes
void init_input_file(int argc, char *argv[]);
void init_output_file(int argc, char *argv[]);
void init_listing_file(void);
void init_temp_files(void);

int validate_names(void);
void generate_extensions(void);
void backup_output(void);

int file_open(FILE **file, const char *filename, const char *mode, int *is_opened);
void file_close(FILE **file, int *is_opened);
void file_delete(FILE **file, const char *filename, int *is_opened);
void file_status(const char *name, const char *filename, FILE *file, int *is_opened);

#endif
