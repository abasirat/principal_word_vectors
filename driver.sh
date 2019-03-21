#!/bin/bash
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
#
#

CORPUS=./cwvec/test/dep_index.txt

CMAT=./cwvec/test/dep_index.bin
WORD_VECTORS=./cwvec/test/dep_index.wvec
EMBEDDINGS=./cwvec/test/dep_index.wembed

CORPUS_TYPE=annotated
CONTEXT_TYPE=indexed

MEM=4

VOCAB=${CORPUS}.vcb
MIN_VCOUNT=100

FEATURE=${CORPUS}.feat
MIN_FCOUNT=100

CWVEC=cwvec/build/cwvec
PWVEC=pwvec/python/pwvec.py

$CWVEC --input $CORPUS --output $CMAT \
  --corpus-type $CORPUS_TYPE \
  --context-type $CONTEXT_TYPE \
  --vocab $VOCAB --min-vcount $MIN_VCOUNT \
  --feature $FEATURE --min-fcount $MIN_FCOUNT \
  --max-memory $MEM --verbose

python3 $PWVEC $CMAT $WORD_VECTORS

paste $VOCAB $WORD_VECTORS |\
    awk '{printf($1) ; for (i=3;i<=NF;i++) printf(" %s", $i) ; printf("\n")}' > $EMBEDDINGS
