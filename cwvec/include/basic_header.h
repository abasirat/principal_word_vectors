/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 * Part of the code is inspired by the implementation of GloVe https://nlp.stanford.edu/projects/glove/
 *
 **/

#ifndef __BASIC_HEADER_H
#define __BASIC_HEADER_H

#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>

#define SENTENCE_BEG  "<s>" 
#define SENTENCE_END  "</s>"
#define UNKNOWN_WORD  "<unk_vocab>" 
#define UNKNOWN_FEAT  "<unk_feature>" 
#define ROOT_CONTEXT  "<root>" 

#define SPACE_DELIM   " \t\n" 
#define COMMA_DELIM   "," 

#define MAX_SENTENCE_LENGTH 1024
#define MAX_FEATURE   64
#define MAX_CONTEXT   64

#define PROGRESS_BREAK  100000
#define ERROR_SIZE      1024

#define OneGigabyte     1073741824

#define BIN_SUFFIX      ".bin"
#define TXT_SUFFIX      ".txt"
#define VCB_SUFFIX      ".vcb"
#define FEAT_SUFFIX     ".feat"

typedef enum { false = 0, true = 1 } bool ;

typedef size_t matrix_index_type ;
typedef double matrix_element_type ;
typedef enum { BIN, TXT } output_format_type ;
typedef enum { BOW, POW, NEIGHBOURHOOD, INDEXED } context_type ;
/* 
 * BOW: Bag-Of-Word. The ordinary bag-of-word window-based context with 1/n weightig function
 * POW: Position-Of-Word. The stack of several NEIGHBOURHOOD contexts 
 * NEIGHBOURHOOD: The neighbourhood context
 * INDEXED: The contexts are indexed in the ANNOTATED input file
 */

typedef enum { RAW, ANNOTATED } corpus_type ;

typedef double hash_value_type ;


#endif
