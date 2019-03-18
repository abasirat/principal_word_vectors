/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 * Part of the code is inspired by the implementation of GloVe https://nlp.stanford.edu/projects/glove/
 *
 **/

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
