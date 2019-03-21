#!/bin/bash
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
#
#

DATA_PREFIX=./cwvec/test/raw # dep_index
CORPUS=${DATA_PREFIX}.txt

CMAT=${DATA_PREFIX}.bin
WORD_VECTORS=${DATA_PREFIX}.wvec
EMBEDDINGS=${DATA_PREFIX}.wembed

CORPUS_TYPE=raw # or annotated
CONTEXT_TYPE=bow # or pow or indexed

MEM=4

VOCAB=${DATA_PREFIX}.vcb
MIN_VCOUNT=100

FEATURE=${DATA_PREFIX}.feat
#MIN_FCOUNT=100 # only with annotated corpora

CWVEC=cwvec/build/cwvec
PWVEC=pwvec/python/pwvec.py

$CWVEC --input $CORPUS --output $CMAT \
  --corpus-type $CORPUS_TYPE \
  --context-type $CONTEXT_TYPE \
  --vocab $VOCAB --min-vcount $MIN_VCOUNT \
  --feature $FEATURE  \
  --normalize \
  --max-memory $MEM --verbose

if [ $? -ne 0 ]; then echo "error while running cwvec "; fi

python3 $PWVEC $CMAT $WORD_VECTORS
if [ $? -ne 0 ]; then echo "error while running pwvec "; fi

paste $VOCAB $WORD_VECTORS |\
    awk '{printf($1) ; for (i=3;i<=NF;i++) printf(" %s", $i) ; printf("\n")}' > $EMBEDDINGS
if [ $? -ne 0 ]; then echo "error while pasting "; fi
