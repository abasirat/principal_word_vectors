#ifndef __TOOLS_H
#define __TOOLS_H

#include "hash.h"
#include "matrix.h"

int matrix_save(char*, matrix_type*, output_format_type) ;

void matrix_print(matrix_type*, FILE*) ;

FILE* my_fopen(const char*, const char*) ;

long fsize(FILE*) ;

hashrec_type** load_hash(FILE*) ;

int save_hash(const char*, hash_table, hash_sort_type) ;

hashrec_type** index_hash(hash_table, hash_sort_type) ;

void error(char*) ;

#endif
