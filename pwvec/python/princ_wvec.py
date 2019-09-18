#
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
# 
#

import sys

import numpy as np
import scipy as sp

import rand_pca as rpca
import matrix_transformer as mt

import pdb

class PrincipalWordVector :
  def __init__(self, cooc_file, embeddings_file, dim=50, transformation="ppmi", max_context=None, feature_selection="frequency", context_file=None) :
    assert(dim > 0)
    self.cooc_file = cooc_file
    try :
      self.cooc = self.load_cooccurrence_matrix()
    except :
      print("cannot load the cooccurrence matrix " + self.cooc_file + "\n", file=sys.stderr) 
    self.cooc = self.cooc.astype('float32') 
    [nr,nc] = self.cooc.shape

    transformer = mt.MatrixTransformer()

    if max_context :
      assert(max_context <= nr) 
      if feature_selection == "frequency" :
        ctx_weight = transformer.frequency_feature_weight(self.cooc)
        idx = ctx_weight.argsort(axis=0)[::-1][0:max_context]
      elif feature_selection == "entropy" :
        #idx = transformer.entropy_feature_weight(self.cooc).argsort(axis=0)[0:max_context]
        [ctx_weight, ent, prb] = transformer.entropy_feature_weight(self.cooc)
        idx = ctx_weight.argsort(axis=0)[0:max_context]
        np.savetxt(context_file + '.ent', ent, fmt='%.3f')
        np.savetxt(context_file + '.prb', prb)
      else :
        raise("Invalid feature selection: %s" % (feature_selection))

      idx = np.array(idx[0:max_context]).flatten() 
      np.savetxt(context_file, idx, fmt='%.0f')

      self.cooc = self.cooc[idx,:]

    if not context_file :
      context_file = embeddings_file + '.ctx'


    print("matrix transformation: [{0}]".format(transformation))
    if transformation == "ppmi" :
      self.cooc = transformer.ppmi(self.cooc)
    if transformation == "Hellinger" :
      self.cooc = transformer.Hellinger(self.cooc)
    if transformation == "MaximumEntropy" :
      self.cooc = transformer.max_entropy(self.cooc)
    print("done")

    [nr,nc] = self.cooc.shape
    print("final cooc size: {0}x{1}".format(nr,nc))
    if nr < dim :
      print("Warning: the size of context unit set ({0}) is smaller than the number of dimensions ({1}).".format(nr,dim))
    
    self.dim = np.min([dim, nr])

    princ_wvec = rpca.RandPca(self.cooc,self.dim).princ_vec.transpose()
    assert(nc == princ_wvec.shape[0])

    if nr < dim:
      print("Warning: the last {0} elements of the vectors are set with zero".format(dim-nr))
      princ_wvec = np.concatenate((princ_wvec, np.zeros([nc, dim - nr])), axis=1) 

    header=str(princ_wvec.shape[0]) + " " + str(self.dim)
    #np.savetxt(embeddings_file, princ_wvec, header=header, comments='', fmt='%.5f') 
    np.savetxt(embeddings_file, princ_wvec, fmt='%.5f') 
    return

  def load_cooccurrence_matrix(self) :
    index_dt = np.uint64 ; # size_t in C
    value_dt = np.float64 ;# long double in C
    cooc_dt = np.dtype([('idx', index_dt, (3,)), ('val', value_dt, 1)])
    data = np.fromfile(self.cooc_file, cooc_dt, -1, "")
    (n_row, n_col, n_hgt) = np.max(data['idx'][:,0:3],0) + 1
    for h in range(0, n_hgt) :
      idx = data['idx'][:,2] == h
      tmp = sp.sparse.csc_matrix((data['val'][idx], (data['idx'][:,0][idx], data['idx'][:,1][idx])), shape = (n_row, n_col))
      if h == 0 :
          cooc = tmp
      else :
          cooc = sp.sparse.vstack([cooc, tmp]) 

    return cooc

