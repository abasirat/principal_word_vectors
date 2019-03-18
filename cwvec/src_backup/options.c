#ifndef __OPTIONS_C
#define __OPTIONS_C

#include "options.h"
//extern char* WARNING_MESSAGE ;

void print_usage() {
  fprintf(stderr,"options --input <file.txt>\n") ;
  fprintf(stderr,"use the following options\n") ;
  fprintf(stderr, "\t--corpus-type\t\t'raw' corpus or 'annotated' corpus (default raw)\n") ;
  fprintf(stderr, "\t--input\t-i\tpath to input file\n") ;
  fprintf(stderr, "\t--output\t-o\tpath to output file (default input.bin or input.txt)\n") ;
  fprintf(stderr, "\t--vocab\t\tpath to vocabulary file (default input.vcb)\n") ;
  fprintf(stderr, "\t--feature\t\tpath to feature file (default input.feat)\n") ;
  fprintf(stderr, "\t--normalize\t-n\tword normalization (all letters are converted to lowecase format and all sequences of digits are replaced with <num>\n") ;
  fprintf(stderr, "\t--context-type\t-c\tthe context type, bow (bag-of-word), pow (position-of-word), neighbourhood, or indexed (default bow)\n") ;
  fprintf(stderr, "\t--load-vocab\t\tload words from vocab file\n") ;
  fprintf(stderr, "\t--load-features\t\tload features from feature file\n") ;
  fprintf(stderr, "\t--output-format\t-f\t'bin' or 'txt' output (default bin)\n") ;
  fprintf(stderr, "\t--max-memory\t-m\tthe amount of memory (in gigabyte) used for fast matrix access. (default 1.0)\n") ;
  fprintf(stderr, "\t--overflow-file\t\toverflow file prefix (default overflow)\n") ;
  fprintf(stderr, "\t--min-vcount\t\tminimum word frequency. Words with frequency smaller than min-vcount are assumed as unknown word (default 1)\n") ;
  fprintf(stderr, "\t--max-vocab\t\tmaximum number of voabulary plus one used for unknown words. Set 0 for infinity. (default 0)\n") ;
  fprintf(stderr, "\t--min-fcount\t\tminimum feature frequency. Feature with frequency smaller than min-fcount are assumed as unknown feature (default 1).\n") ;
  fprintf(stderr, "\t--max-feature\t\tmaximum number of features plus one used for unknown feature. Set 0 for infinity. (default 0)\n") ;
  fprintf(stderr, "\t--window\t-w\twindow size (default -1)\n") ;
  fprintf(stderr, "\t--symmetric\t\tsymmetric window of size window_size)\n") ;
  fprintf(stderr, "\t--print\t\tprint cooccurrence matrix on standard output\n") ;
  fprintf(stderr, "\t--verbose\t-v\tenable verbose\n") ;
  fprintf(stderr, "\t--help\t-h\tprint this message\n") ;
}


int validate_options( struct input_options_st *options ) {
  if (!options->input_path) {
    fprintf(stderr, "no corpus!\n") ;
    return -1 ;
  }

  if (!options->output_path) {
    char* suffix = NULL ;
    if (options->output_format == BIN) suffix = BIN_SUFFIX ;
    else if (options->output_format == TXT) suffix = TXT_SUFFIX ;

    options->output_path = (char*)malloc(strlen(options->input_path) + strlen(suffix) + 1);
    sprintf(options->output_path, "%s%s", options->input_path, suffix) ;
  }

  if (!options->vocabulary_path) {
    options->vocabulary_path = (char*)malloc(strlen(options->input_path) + strlen(VCB_SUFFIX) + 1);
    sprintf(options->vocabulary_path, "%s%s", options->input_path, VCB_SUFFIX) ;
  }

  if (!options->feature_path) {
    options->feature_path = (char*)malloc(strlen(options->input_path) + strlen(FEAT_SUFFIX) + 1);
    sprintf(options->feature_path, "%s%s", options->input_path, FEAT_SUFFIX) ;
  }


  if (options->corpus == RAW) {
    if (options->context == INDEXED) {
      sprintf(ERROR_MESSAGE, "validate_options: inconsistent options 'indexed' context and 'raw' corpus!") ;
      return -1 ;
    }

    if (options->min_fcount != 1) {
      sprintf(ERROR_MESSAGE, "validate_options: min-fcount is not implemented for raw corpus, yet!") ;
      return -1 ;
    }

    if (options->max_feature != 0) {
      sprintf(ERROR_MESSAGE, "validate_options: max_feature is not implemented for raw corpus, yet!") ;
      return -1 ;
    }

  }
  else if (options->corpus == ANNOTATED) {
  }

  if ( ( options->load_vocabulary == true ) && !options->vocabulary_path ) {
    sprintf(ERROR_MESSAGE, "validate_options: inconsistent options 'load_vocabulary' and 'vocabulary_path == NULL'!") ;
    return -1 ;
  }


  if ( ( options->load_features == true ) && !options->feature_path ) {
    sprintf(ERROR_MESSAGE, "validate_options: inconsistent options 'load_features' and 'feature_path == NULL'!") ;
    return -1 ;
  }

  return 0 ;
}

void init_options(struct input_options_st *options) {
  options->corpus      = RAW ;
  options->input_path       = NULL ;
  options->output_path      = NULL ;
  options->vocabulary_path  = NULL ;
  options->feature_path     = NULL ;
  options->normalization    = false ;
  options->context     = BOW ;
  options->load_vocabulary  = false ;
  options->load_features    = false ;
  options->output_format    = BIN ;
  options->max_memory       = 1*OneGigabyte ;
  options->overflow_file    = "overflow" ;
  options->min_vcount       = 1 ;
  options->max_vocab        = 0 ;
  options->min_fcount       = 1 ;
  options->max_feature      = 0 ;
  options->window_size      = -1 ;
  options->symmetric = false ;
  options->verbose          = false ;
  options->print            = false ;
}

void delete_input_options(struct input_options_st *options) {
  if (options->input_path)      free(options->input_path) ;
  if (options->output_path)     free(options->output_path) ;
  if (options->vocabulary_path) free(options->vocabulary_path) ;
  if (options->feature_path)    free(options->feature_path) ;
}

int parse_options(int argc, char* argv[], struct input_options_st *options) {
  char short_options_str[1024] ;
  int i, j ;

  enum short_options_enum {
    CORPUS_TYPE_OPTION      = 1000,
    INPUT_PATH_OPTION       = 'i',
    OUTPUT_PATH_OPTION      = 'o',
    VOCABULARY_PATH_OPTION  = 1001,
    FEATURE_PATH_OPTION     = 1002,
    NORMALIZATION_OPTION    = 'n',
    CONTEXT_TYPE_OPTION     = 'c',
    LOAD_VOCABULARY_OPTION  = 1003,
    LOAD_FEATURES_OPTION    = 1004,
    OUTPUT_FORMAT_OPTION    = 'f',
    MAX_MEMORY_OPTION       = 'm',
    OVERFLOW_FILE_OPTION    = 1005,
    MIN_VCOUNT_OPTION       = 1006,
    MAX_VOCAB_OPTION        = 1007,
    MIN_FCOUNT_OPTION       = 1008,
    MAX_FEATURE_OPTION      = 1009,
    WINDOW_SIZE_OPTION      = 'w',
    SYMMETRIC_WINDOW_OPTION = 1010,
    VERBOSE_OPTION          = 'v',
    PRINT_OPTION            = 1011,
    HELP_OPTION             = 'h',
  } ;

  static struct option long_options[] = {
    {"corpus-type",     required_argument,  0,  CORPUS_TYPE_OPTION },
    {"input",           required_argument,  0,  INPUT_PATH_OPTION },
    {"output",          required_argument,  0,  OUTPUT_PATH_OPTION },
    {"vocab",           required_argument,  0,  VOCABULARY_PATH_OPTION },
    {"feature",         required_argument,  0,  FEATURE_PATH_OPTION },
    {"normalize",       no_argument,        0,  NORMALIZATION_OPTION },
    {"context-type",    required_argument,  0,  CONTEXT_TYPE_OPTION },
    {"load-vocab",      no_argument,        0,  LOAD_VOCABULARY_OPTION  },
    {"load-features",   no_argument,        0,  LOAD_FEATURES_OPTION },
    {"output-format",   required_argument,  0,  OUTPUT_FORMAT_OPTION },
    {"max-memory",      required_argument,  0,  MAX_MEMORY_OPTION },
    {"overflow_file",   required_argument,  0,  OVERFLOW_FILE_OPTION },
    {"min-vcount",      required_argument,  0,  MIN_VCOUNT_OPTION },
    {"max-vocab",       required_argument,  0,  MAX_VOCAB_OPTION },
    {"min-fcount",      required_argument,  0,  MIN_FCOUNT_OPTION },
    {"max-feature",     required_argument,  0,  MAX_FEATURE_OPTION },
    {"window-size",     required_argument,  0,  WINDOW_SIZE_OPTION },
    {"symmetric",       no_argument,  0,  SYMMETRIC_WINDOW_OPTION },
    {"verbose",         no_argument,  0,  VERBOSE_OPTION },
    {"print",           no_argument,  0,  PRINT_OPTION },
    {"help",            no_argument,        0,  HELP_OPTION},
    {0,                 0,                  0,  0 }
  };

  setlocale(LC_ALL, "") ;
  for (i = 0 , j = 0 ; long_options[i].name ; i++, j++) {
    short_options_str[j] = (char)long_options[i].val ;
    if (long_options[i].has_arg == required_argument)
      short_options_str[++j] = ':' ;
  }
  short_options_str[j] = '\0' ;

  init_options(options) ;

  while(1) {
    int option_index = 0;

    int c = getopt_long(argc, argv, (const char*)short_options_str, long_options, &option_index) ;
    if (c == -1) break ;
    switch(c) {
      case 0:
        fprintf(stderr, "option %s", long_options[option_index].name);
        if (optarg) fprintf(stderr, " with arg %s", optarg);
        fprintf(stderr, "\n");
        break ;
      case CORPUS_TYPE_OPTION :
        if ( !strcmp(optarg, "raw") ) options->corpus = RAW ;
        else if (!strcmp(optarg, "annotated")) options->corpus = ANNOTATED ;
        else {
          fprintf(stderr, "invalid corpus type %s.\n", optarg) ;
          print_usage() ;
          return -1 ;
        }
        break ;
      case INPUT_PATH_OPTION :
        options->input_path = strdup(optarg) ;
        break ;
      case OUTPUT_PATH_OPTION :
        options->output_path = strdup(optarg) ;
        break ;
      case VOCABULARY_PATH_OPTION :
       options-> vocabulary_path = strdup(optarg) ;
        break ;
      case FEATURE_PATH_OPTION :
       options-> feature_path = strdup(optarg) ;
        break ;
      case NORMALIZATION_OPTION :
        options->normalization = true ;
        break ;
      case CONTEXT_TYPE_OPTION :
        if ( !strcmp(optarg, "bow") ) options->context = BOW ;
        else if ( !strcmp(optarg, "pow") ) options->context = POW ;
        else if ( !strcmp(optarg, "neighbourhood") ) options->context = NEIGHBOURHOOD ;
        else if ( !strcmp(optarg, "indexed") ) options->context = INDEXED ;
        else {
          fprintf(stderr, "invalid context type %s.\n", optarg) ;
          print_usage() ;
          return -1 ;
        }
        break ;
      case LOAD_VOCABULARY_OPTION :
        options->load_vocabulary = true ;
        break ;
      case LOAD_FEATURES_OPTION :
        options->load_features = true ;
        break ;
      case OUTPUT_FORMAT_OPTION :
        if (!strcmp(optarg, "txt")) options->output_format = TXT ;
        else if (!strcmp(optarg, "bin")) options->output_format = BIN ;
        else {
          fprintf(stderr, "invalid output format %s.\n", optarg) ;
          print_usage() ;
          return -1 ;
        }
        break ;
      case MAX_MEMORY_OPTION :
        options->max_memory = strtod((optarg), NULL)*OneGigabyte ;
        break ;
      case OVERFLOW_FILE_OPTION :
        options->overflow_file = strdup(optarg) ;
        break ;
      case MIN_VCOUNT_OPTION :
        options->min_vcount = (size_t)atol(optarg) ;
        break ;
      case MAX_VOCAB_OPTION :
        options->max_vocab = (size_t)atol(optarg) ;
        break ;
      case MIN_FCOUNT_OPTION :
        options->min_fcount = (size_t)atol(optarg) ;
        break ;
      case MAX_FEATURE_OPTION :
        options->max_feature = (size_t)atol(optarg) ;
        break ;
      case WINDOW_SIZE_OPTION :
        options->window_size = atoi(optarg) ;
        break ;
      case SYMMETRIC_WINDOW_OPTION :
        options->symmetric = true ;
        break ;
      case VERBOSE_OPTION :
        options->verbose = true ;
        break ;
      case PRINT_OPTION :
        options->print = true ;
        break ;
      case HELP_OPTION :
        print_usage() ;
        return 1 ;
      default:
        fprintf(stderr, "?? getopt returned character code 0%o ??\n", c);
        return -2 ;
    }
  }

  if (validate_options(options) < 0) {
    fprintf(stderr, "invalid option!\n") ;
    return -3 ;
  }

  /**************
   * This should be removed after min-fcount and max-features are implemented for raw corpus.
   *************/
  if (options->corpus == RAW) {
    options->min_fcount = options->min_vcount ;
    options->max_feature = options->max_vocab ;
  }
  /*************/

  return 0 ;
}

void print_input_options(FILE* fid, const struct input_options_st *options) {
  char* tmp ;
  if (!options) return ;
  fprintf(fid, "corpus: %s\n", options->corpus == RAW ? "raw" : "annotated") ;
  fprintf(fid, "input_path: %s\n", options->input_path == NULL ? "" : options->input_path) ;
  fprintf(fid, "output_path: %s\n", options->output_path == NULL ? "" : options->output_path) ;
  fprintf(fid, "vocabulary_path: %s\n", options->vocabulary_path == NULL ? "" : options->vocabulary_path) ;
  fprintf(fid, "feature_path: %s\n", options->feature_path == NULL ? "" : options->feature_path) ;
  fprintf(fid, "normalization: %s\n", options->normalization == true ? "true" : "false") ;
  switch (options->context) {
    case BOW: tmp = "BOW" ; break ;
    case POW: tmp = "POW" ; break ;
    case NEIGHBOURHOOD: tmp = "NEIGHBOUTHOOD" ; break ;
    case INDEXED: tmp = "INDEXED" ; break ;
    default: tmp = NULL ;
  }
  fprintf(fid, "context: %s\n", tmp == NULL ? "" : tmp) ;
  fprintf(fid, "load_vocabulary: %s\n", options->load_vocabulary == true ? "true" : "false") ;
  fprintf(fid, "load_features: %s\n", options->load_features == true ? "true" : "false") ;
  fprintf(fid, "output_format: %s\n", options->output_format == BIN ? "bin" : "txt") ;
  fprintf(fid, "max_memory: %e\n", options->max_memory) ;
  fprintf(fid, "min_vcount: %zu\n", options->min_vcount) ;
  fprintf(fid, "max_vocab: %zu\n", options->max_vocab) ;
  fprintf(fid, "min_fcount: %zu\n", options->min_fcount) ;
  fprintf(fid, "max_featue: %zu\n", options->max_feature) ;
  fprintf(fid, "window_size: %d\n", options->window_size) ;
  fprintf(fid, "symmetric: %s\n", options->symmetric == true ? "true" : "false") ;
  fprintf(fid, "verbose: %s\n", options->verbose == true ? "true" : "false") ;
  fprintf(fid, "print: %s\n", options->print == true ? "true" : "false") ;
}

#endif
