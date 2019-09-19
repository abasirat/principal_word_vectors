#
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
# 
#

import numpy as np
from scipy import sparse
from scipy.optimize import minimize, LinearConstraint
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
 
    diag = 1.0 / np.array(A.sum(0)).flatten() 
    diag[diag == np.inf] = 0
    pr = sparse.diags(diag) 

    P = A.dot(pr)

    logP = copy.deepcopy(P)
    logP.data = np.log(logP.data)
    
    ent = -(P.multiply(logP)).sum(0)
    mean = np.mean(ent)
    var = np.var(ent)
    #weight = np.multiply(pr.diagonal().reshape([nr,1]) , np.exp(-(ent - mean)/(2*var)))
    #weight = np.multiply(pr.diagonal().reshape([nr,1]) , 1.0 / np.log(ent).flatten())
    weight = np.multiply(np.log(pr.diagonal().reshape([nr,1])) , 1.0 /np.log(ent))

    return weight, ent, pr.diagonal()

  def frequency_feature_weight(self, A) :
    [nr,nc] = A.shape
 
    weight = np.array(A.sum(0)).flatten() 

    return weight

  def Hellinger(self, A) :
    A = self.colnormalize(A)
    A.data = np.sqrt(A.data) 
    return A

  def normalize(self, A) :
    [nr,nc] = A.shape
    r = sparse.eye(nr)
    with np.errstate(divide='ignore'):
      diag = 1.0 / np.array(A.sum(1)).flatten() 
    diag[diag == np.inf] = 0
    r.setdiag(diag) 

    c = sparse.eye(nc)
    with np.errstate(divide='ignore'):
      diag = 1.0 / np.array(A.sum(0)).flatten() 
    diag[diag == np.inf] = 0
    c.setdiag(diag) 

    A = r.dot(A).dot(c)
    return A

  def colnormalize(self, A) :
    [nr,nc] = A.shape
    c = sparse.eye(nc)
    with np.errstate(divide='ignore'):
      diag = 1.0 / np.array(A.sum(0)).flatten() 
    diag[diag == np.inf] = 0
    c.setdiag(diag) 

    A = A.dot(c)
    return A

  def rownormalize(self, A) :
    [nr,nc] = A.shape
    r = sparse.eye(nr)
    with np.errstate(divide='ignore'):
      diag = 1.0 / np.array(A.sum(1)).flatten() 
    diag[diag == np.inf] = 0
    r.setdiag(diag) 

    A = r.dot(A)
    return A



  def ppmi(self, A) :
    assert(sparse.issparse(A))

    [nr,nc] = A.shape
    T = A.sum()

    phi = sparse.eye(nr)
    with np.errstate(divide='ignore'):
      diag = 1.0 / np.array(A.sum(0)).flatten()
    diag[diag == np.inf] = 0
    phi.setdiag(diag) 

    omega = sparse.eye(nc)
    with np.errstate(divide='ignore'):
      diag = 1.0 / np.array(A.sum(1)).flatten() 
    diag[diag == np.inf] = 0
    omega.setdiag(diag) 
    
    A = T*phi.dot(A.dot(omega)) ;
    A = A.log1p() ; # instead of max(log(A),0), we take log(A+1)
    return A

  def entropy(self, A):
    assert(sparse.issparse(A))
    [prb, bins] = np.histogram(A.data, density=True)
    with np.errstate(divide='ignore'):
      log_prb = np.log(prb)/np.log(2)
    log_prb[np.isinf(log_prb)] = 0
    ent = -prb.dot(log_prb.T)
    return ent

  def max_entropy(self, A) :
    assert(sparse.issparse(A))
    A = self.ppmi(A)
    neg_entropy = lambda x: -self.entropy(A.power(x[0]))
    x0 = np.array([1/7])
    constraint = LinearConstraint(np.array([1]), np.array([0]), np.array([1]), keep_feasible=False)
    res = minimize(neg_entropy, x0, method='SLSQP', constraints=(constraint), options={'disp': False})
    print("The optimal power value is: {0:.3f} [Entropy={1:.3f}]".format(res.x[0], -res.fun))
    return A.power(res.x[0]) 

