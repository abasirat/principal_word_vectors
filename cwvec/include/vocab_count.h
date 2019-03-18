#ifndef __VOCAB_COUNT_H
#define __VOCAB_COUNT_H

hash_table prune_vocabulary(hash_table vocab_hash, size_t min_count, size_t max_vocab, char* unk) ;
hashrec_type** load_vocabulary(const char *path) ;
int raw_build_vocabulary(const char *path, hashrec_type ***vh, hashrec_type ***fh, bool normalization) ;
int annotated_build_vocabulary(const char *path, hashrec_type ***vh, hashrec_type ***fh, bool normalization) ;

//int build_vocabulary(const struct input_options_st *options, hashrec_type ***vh, hashrec_type ***fh) ;
#endif
