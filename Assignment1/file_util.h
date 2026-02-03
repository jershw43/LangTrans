#ifndef FILE_UTIL_H
#define FILE_UTIL_H
#include <stdio.h>
#include <stdbool.h>
//Define constants:
#define MAX_FILENAME 256
#define IN ".IN"
#define OUT ".OUT"
#define LIS ".LIS"
#define BAK ".BAK"




void open_input_file();
void open_output_file();
void open_listing_file();
void create_temp_files();
void validate_names();
void generate_extensions();
void backup_output();

#endif
