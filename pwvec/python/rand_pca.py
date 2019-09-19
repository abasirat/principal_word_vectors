#!/usr/bin/env python

import pdb
import numpy as np
from scipy import linalg, sparse 
from scipy.fftpack import fft
from math import ceil

class RandPca :
  def __init__(self, A, k, q=0) :
    self.princ_vec = self.rpca(A, k, q)
    return

  def randomized_subspace_iteration(self, A, l, q, u) : 
    (m,n) = A.shape
    l = np.min([m,l])
    Y = A.dot(np.random.normal(size=[n,l]))
    
    [Q,R] = np.linalg.qr(Y, mode='reduced')

    Q = Q.astype('float32') 
    R = R.astype('float32') 
    u = u.astype('float32') 
    v = np.ones([R.shape[1],1], dtype='float32')
    o = np.ones([n,1], dtype='float32')

    if np.sum(u) != 0: 
      [Q,R] = linalg.qr_update(Q, R, u, v)
    for i in range(q):
      [G,R] = np.linalg.qr(A.T.dot(Q) + o.dot(u.T.dot(Q)), mode='reduced')
      [Q,R] = np.linalg.qr(A.dot(G) + u.dot(o.T.dot(G)), mode='reduced')
    return Q,R

  def rsvd(self, A, k, q, l, centre = True) :
    (m,n) = A.shape
    l = np.max([l, 2*k]) 
    E = A.mean(1).reshape([m,1])
    u = -E if centre else np.zeros(E.shape)
    
    [Q,R] = self.randomized_subspace_iteration(A, l, q, u)
    B = (A.T.dot(Q) - (E.T.dot(Q) if centre else 0)).T

    [U,S,V] = linalg.svd(B, full_matrices=False)
    U = Q.dot(U)
    k = np.min([k, U.shape[1]])
    U = U[:,0:k]
    S = S[0:k]
    V = V[0:k,:]
    return U, S, V

  def rpca(self, A, k, q=0, centre = True) :
    (m,n) = A.shape
    assert(k <= m)
    [U,S,V] = self.rsvd(A, k, q, 2*k, centre)
    return V
    #return sparse.diags(S).dot(V)


