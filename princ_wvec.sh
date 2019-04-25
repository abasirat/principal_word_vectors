#!/bin/bash
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
#
#

DATA_PREFIX=./cwvec/test/dep_index # raw dep_index or raw
CORPUS=${DATA_PREFIX}.txt

CMAT=${DATA_PREFIX}.bin
WORD_VECTORS=${DATA_PREFIX}.wvec
EMBEDDINGS=${DATA_PREFIX}.wembed

CORPUS_TYPE=annotated # raw or annotated
CONTEXT_TYPE=bow # or pow or indexed

MEM=4

VOCAB=${DATA_PREFIX}.vcb
MIN_VCOUNT=100

FEATURE=${DATA_PREFIX}.feat
#MIN_FCOUNT=100 # only with annotated corpora

WINDOW=2

CWVEC=cwvec/build/cwvec
PWVEC=pwvec/python/pwvec.py

FEATURE_SELECTION=frequency # or entropy

TRANSFORMATION=ppmi #ppmi, Hellinger, or MaximumEntropy

$CWVEC --input $CORPUS --output $CMAT \
  --corpus-type $CORPUS_TYPE \
  --window $WINDOW \
  --context-type $CONTEXT_TYPE \
  --vocab $VOCAB --min-vcount $MIN_VCOUNT \
  --feature $FEATURE  \
  --normalize \
  --max-memory $MEM --verbose

if [ $? -ne 0 ]; then echo "error while running cwvec "; fi

python3 $PWVEC $CMAT $WORD_VECTORS $FEATURE_SELECTION $TRANSFORMATION
if [ $? -ne 0 ]; then echo "error while running pwvec "; fi

paste $VOCAB $WORD_VECTORS |\
    awk '{printf($1) ; for (i=3;i<=NF;i++) printf(" %s", $i) ; printf("\n")}' > $EMBEDDINGS
if [ $? -ne 0 ]; then echo "error while pasting "; fi
