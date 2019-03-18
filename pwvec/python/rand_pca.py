#!/usr/bin/env python

import pdb
import numpy as np
from scipy import linalg, sparse

class RandPca :
  def __init__(self, A, k) :
    self.princ_vec = self.rpca(A, k)
    return 

  def randomized_subspace_iteration(self, A, l, q) : 
    (m,n) = A.shape
    l = np.min([m,l])
    Y = A.dot(np.random.normal(size=[n,l]))
    [Q,R] = np.linalg.qr(Y, mode='reduced')
    return Q,R

  def rsvd(self, A, k, q, l, centre=True, negsmp=1) :
    assert(negsmp >= 1)

    (m,n) = A.shape

    E = np.log(negsmp) * np.ones([m,1]) ;
    if centre: E += A.mean(1).reshape([m,1]) 

    l = np.max([l, 2*q]) 
    [Q,R] = self.randomized_subspace_iteration(A, l, q)
    O = np.ones([R.shape[1], 1])
    [Q,R] = linalg.qr_update(Q, R, -E, O)
    B = (A.transpose().dot(Q) - E.transpose().dot(Q)).transpose() 
    [U,S,V] = linalg.svd(B, full_matrices=False)
    U = Q.dot(U)
    k = np.min([k, U.shape[1]])
    U = U[:,0:k]
    S = S[0:k]
    V = V[0:k,:]
    return U, S, V

  def rpca(self, A, k) :
    (m,n) = A.shape
    assert(k <= m)

    [U,S,V] = self.rsvd(A, k, 1, 2*k)

    return sparse.diags(S).dot(V)


