function [Q R] = randomized_subspace_iteration_1(A, l, q)
[M, N] = size(A) ;
l = min(l, M) ;
Y = A * randn(N, l) ;
[Q, R] = qr(Y, 0) ;

for j=1:q
  Y = A * (A' * Q) ;
  [Q, R] = qr(Y, 0) ;
end
