#!/bin/bash
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
#
#

# before running this from the terminal for the first time

# pip3 install numpy
# pip3 install scipy 

# cd cwvec
# make

# if raw, it is fine as long as the words are separated by spaces
# 


DATA_PREFIX=./cwvec/test/corpus.data # raw dep_index or raw
CORPUS=${DATA_PREFIX}.txt

CMAT=${DATA_PREFIX}.bin
WORD_VECTORS=${DATA_PREFIX}.wvec
EMBEDDINGS=${DATA_PREFIX}.wembed

CORPUS_TYPE=raw # raw or annotated
CONTEXT_TYPE=bow # or pow position-of-words or indexed, bow bag-of-words means the order does not count

MEM=4

VOCAB=${DATA_PREFIX}.vcb
MIN_VCOUNT=10

FEATURE=${DATA_PREFIX}.feat
#MIN_FCOUNT=100 # only with annotated corpora

# set +5 for words after the center, set -5 for the preceding words (e.g., I [want] an apple, I is -1
WINDOW=3

CWVEC=cwvec/build/cwvec
PWVEC=pwvec/python/pwvec.py

FEATURE_SELECTION=frequency # or entropy

TRANSFORMATION=MaximumEntropy #ppmi, Hellinger, or MaximumEntropy

$CWVEC --input $CORPUS --output $CMAT \
  --corpus-type $CORPUS_TYPE \
  --window $WINDOW \
  --context-type $CONTEXT_TYPE \
  --vocab $VOCAB --min-vcount $MIN_VCOUNT \
  --feature $FEATURE  \
  --max-memory $MEM --verbose
# this line can be removed if you run on Chinese
# --normalize \ 

if [ $? -ne 0 ]; then echo "error while running cwvec "; fi

python3 $PWVEC $CMAT $WORD_VECTORS $FEATURE_SELECTION $TRANSFORMATION
if [ $? -ne 0 ]; then echo "error while running pwvec "; fi

paste $VOCAB $WORD_VECTORS |\
    awk '{printf("%s",$1) ; for (i=3;i<=NF;i++) printf(" %s", $i) ; printf("\n")}' > $EMBEDDINGS
if [ $? -ne 0 ]; then echo "error while pasting "; fi
