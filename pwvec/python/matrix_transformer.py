#
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
# 
#

import numpy as np
from scipy import sparse
import copy

import pdb

class MatrixTransformer :
  def __init__(self) :
    return 

  def spfun(self, function, A) :
    A.data = function(A.data)
    return A

  def entropy_feature_weight(self, A) :
    [nr,nc] = A.shape
 
    diag = 1.0 / np.array(A.sum(1)).flatten() 
    diag[diag == np.inf] = 0
    pr = sparse.diags(diag) 

    P = A.dot(pr)

    logP = copy.deepcopy(P)
    logP.data = np.log(logP.data)
    
    ent = -(P.multiply(logP)).sum(1)
    mean = np.mean(ent)
    var = np.var(ent)
    #weight = np.multiply(pr.diagonal().reshape([nr,1]) , np.exp(-(ent - mean)/(2*var)))
    #weight = np.multiply(pr.diagonal().reshape([nr,1]) , 1.0 / np.log(ent).flatten())
    weight = np.multiply(np.log(pr.diagonal().reshape([nr,1])) , 1.0 /np.log(ent))

    return weight, ent, pr.diagonal()

  def frequency_feature_weight(self, A) :
    [nr,nc] = A.shape
 
    weight = np.array(A.sum(1)).flatten() 

    return weight

  def Hellinger(self, A) :
    A.data = np.log(A.data) 
    return A

  def ppmi(self, A) :
    assert(sparse.issparse(A))

    [nr,nc] = A.shape
    T = A.sum()

    phi = sparse.eye(nr)
    diag = 1.0 / np.array(A.sum(1)).flatten() 
    diag[diag == np.inf] = 0
    phi.setdiag(diag) 

    omega = sparse.eye(nc)
    diag = 1.0 / np.array(A.sum(2)).flatten() 
    diag[diag == np.inf] = 0
    omega.setdiag(diag) 
    
    A = T*phi.dot(A.dot(omega)) ;
    A = A.log1p() ; # instead of max(log(A),0), we take log(A+1)
    return A

