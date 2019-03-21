#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
#
#

import sys
import princ_wvec as pwvec

cooc_file=sys.argv[1]
emb_file=sys.argv[2]

embeddings = pwvec.PrincipalWordVector(cooc_file, emb_file)
