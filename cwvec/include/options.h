/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 * Part of the code is inspired by the implementation of GloVe https://nlp.stanford.edu/projects/glove/
 *
 **/

#ifndef __OPTIONS_H
#define __OPTIONS_H

#include "basic_header.h"

#include <string.h>
#include <getopt.h>
#include <locale.h>


extern char ERROR_MESSAGE[] ;

struct input_options_st {
  corpus_type corpus ;
  char* input_path ;
  char* output_path ;
  char* vocabulary_path ;
  char* feature_path ;
  bool normalization ;
  context_type context ;
  bool load_vocabulary ;
  bool load_features ;
  output_format_type output_format ;
  double max_memory ;
  char *overflow_file ;
  size_t min_vcount ;
  size_t max_vocab ;
  size_t min_fcount ;
  size_t max_feature ;
  int window_size ; 
  bool symmetric ;
  bool verbose ;
  bool print ;
};

void print_input_options(FILE*, const struct input_options_st*) ;
void init_options(struct input_options_st*) ;
void delete_input_options(struct input_options_st*) ;
int parse_options(int, char**, struct input_options_st*) ;
void print_usage() ;

#endif


