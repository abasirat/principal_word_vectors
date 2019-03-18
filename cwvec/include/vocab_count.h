/**
 *
 * This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
 * project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
 * You are allowed to modify or distribute it if you keep this header part
 * 
 * Part of the code is inspired by the implementation of GloVe https://nlp.stanford.edu/projects/glove/
 *
 **/

#ifndef __VOCAB_COUNT_H
#define __VOCAB_COUNT_H

hash_table prune_vocabulary(hash_table vocab_hash, size_t min_count, size_t max_vocab, char* unk) ;
hashrec_type** load_vocabulary(const char *path) ;
int raw_build_vocabulary(const char *path, hashrec_type ***vh, hashrec_type ***fh, bool normalization) ;
int annotated_build_vocabulary(const char *path, hashrec_type ***vh, hashrec_type ***fh, bool normalization) ;

//int build_vocabulary(const struct input_options_st *options, hashrec_type ***vh, hashrec_type ***fh) ;
#endif
