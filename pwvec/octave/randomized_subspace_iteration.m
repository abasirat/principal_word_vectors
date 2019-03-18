%
%
% This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
% project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
% You are allowed to modify or distribute it if you keep this header part
% 
%

function [Q R] = randomized_subspace_iteration_1(A, l, q)
[M, N] = size(A) ;
l = min(l, M) ;
Y = A * randn(N, l) ;
[Q, R] = qr(Y, 0) ;

for j=1:q
  Y = A * (A' * Q) ;
  [Q, R] = qr(Y, 0) ;
end
